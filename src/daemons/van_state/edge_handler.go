package edge_handler

import (
	"log"

	consts "github.com/markbradley27/henrietta/src/consts/go_consts"
	van_state_proto "github.com/markbradley27/henrietta/src/proto/van_state_go_proto"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	"github.com/warthog618/gpiod"
	prototext "google.golang.org/protobuf/encoding/prototext"
	proto "google.golang.org/protobuf/proto"
)

type EdgeHandler struct {
	mqttClient mqtt.Client
}

func NewEdgeHandler(mqttClient mqtt.Client) *EdgeHandler {
	return &EdgeHandler{mqttClient}
}

func (eh *EdgeHandler) HandleEngineRunEdge(event gpiod.LineEvent) {
	log.Printf("Handling edge event: %v", event)

	var engineState van_state_proto.VanState_EngineState
	if event.Type == gpiod.LineEventRisingEdge {
		engineState = van_state_proto.VanState_ENGINE_RUNNING
	} else {
		engineState = van_state_proto.VanState_ENGINE_OFF
	}
	vanState := van_state_proto.VanState{
		EngineState: &engineState,
	}

	vanStateText, err := prototext.Marshal(&vanState)
	if err != nil {
		log.Printf("Error text formatting VanState message: %v", err)
		return
	}
	log.Printf("Publishing: %s", vanStateText)

	vanStateBytes, err := proto.Marshal(&vanState)
	if err != nil {
		log.Printf("Error marshalling VanState proto: %v", err)
		return
	}

	token := eh.mqttClient.Publish(consts.TopicStateVan, 2, false, vanStateBytes)
	token.Wait()
	if token.Error() != nil {
		log.Printf("Error publishing VanState proto: %v", token.Error())
	}
}
