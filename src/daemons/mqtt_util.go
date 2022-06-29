package mqtt_util

import (
	"fmt"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
)

func ConnectMQTTClient(host string, port int, clientID string) (mqtt.Client, error) {
	addr := fmt.Sprintf("tcp://%s:%d", host, port)
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
