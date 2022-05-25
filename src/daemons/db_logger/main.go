package main

import (
	"flag"
	"fmt"
	"log"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	consts "github.com/henrietta/src/consts/go_consts"
)

func main() {
	mqttHost := flag.String("mqtt_host", consts.RpiIp, "MQTT broker host.")
	mqttPort := flag.Int("mqtt_port", consts.MosquittoPort, "MQTT broker port.")
	mqttClientID := flag.String("mqtt_client_id", consts.ClientIdDbLogger, "MQTT client id.")
	flag.Parse()

	mqttAddr := fmt.Sprintf("tcp://%s:%d", *mqttHost, *mqttPort)
	log.Printf("Connecting to MQTT server at %#v as %#v...", mqttAddr, *mqttClientID)
	mqttClient, err := ConnectMQTTClient(mqttAddr, *mqttClientID)
	if err != nil {
		log.Fatalf("Connecting to MQTT: %v", err)
	}
	mqttClientOptions := mqttClient.OptionsReader()
	log.Printf("Connected; ClientID: %#v.", mqttClientOptions.ClientID())

	log.Printf("Subscribing to %#v.", consts.TopicMetricsEnviro)
	mqttClient.Subscribe(consts.TopicMetricsEnviro, 2, HandleMetricEnviro)

	select {}
}

func ConnectMQTTClient(addr, clientID string) (mqtt.Client, error) {
	options := mqtt.NewClientOptions()
	options.AddBroker(addr)
	options.SetClientID(clientID)
	client := mqtt.NewClient(options)

	token := client.Connect()
	token.WaitTimeout(30 * time.Second)
	if token.Error() != nil {
		return nil, token.Error()
	}
	if !client.IsConnected() {
		return nil, fmt.Errorf("connection timed out")
	}
	return client, nil
}

func HandleMetricEnviro(client mqtt.Client, message mqtt.Message) {
	log.Printf("Handling topic %#v message: %s", message.Topic(), message.Payload())
	message.Ack()
}
