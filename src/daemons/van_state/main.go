package main

import (
	"flag"
	"log"
	"time"

	consts "github.com/markbradley27/henrietta/src/consts/go_consts"
	"github.com/markbradley27/henrietta/src/daemons/log_util"
	"github.com/markbradley27/henrietta/src/daemons/mqtt_util"
	"github.com/markbradley27/henrietta/src/daemons/van_state/edge_handler"

	"github.com/warthog618/gpiod"
)

const (
	engineRunPinDebouncePeriod = 1 * time.Second
	engineRunPinNumber         = 17
	gpioChipDevName            = "gpiochip0"
)

func main() {
	logFilePath := flag.String("log_file", "", "Log file.")

	mqttHost := flag.String("mqtt_host", consts.RpiIp, "MQTT broker host.")
	mqttPort := flag.Int("mqtt_port", consts.MosquittoPort, "MQTT broker port.")
	mqttClientID := flag.String("mqtt_client_id", consts.ClientIdVanState, "MQTT client id.")
	flag.Parse()

	log_util.SetupLogFile(*logFilePath)

	log.Printf("Connecting to MQTT server at %s:%d as %#v...", *mqttHost, *mqttPort, *mqttClientID)
	mqttClient, err := mqtt_util.ConnectMQTTClient(*mqttHost, *mqttPort, *mqttClientID)
	if err != nil {
		log.Fatalf("Connecting to MQTT: %v", err)
	}
	defer mqttClient.Disconnect(0)
	log.Print("Connected.")

	edgeHandler := edge_handler.NewEdgeHandler(mqttClient)

	log.Printf("Subscribing to engine run pin %d edges...", engineRunPinNumber)
	engineRunPin, err := gpiod.RequestLine(gpioChipDevName, engineRunPinNumber, gpiod.WithEventHandler(edgeHandler.HandleEngineRunEdge), gpiod.WithBothEdges, gpiod.WithDebounce(engineRunPinDebouncePeriod))
	if err != nil {
		log.Fatalf("Requesting engine run pin: %v", err)
	}
	defer engineRunPin.Close()
	log.Print("Subscribed.")

	select {}
}
