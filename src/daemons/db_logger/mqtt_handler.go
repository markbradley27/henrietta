package mqtt_handler

import (
	"database/sql"
	"log"
	"time"

	enviro_proto "github.com/markbradley27/henrietta/src/proto/enviro_go_proto"
	van_state_proto "github.com/markbradley27/henrietta/src/proto/van_state_go_proto"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	prototext "google.golang.org/protobuf/encoding/prototext"
	proto "google.golang.org/protobuf/proto"
)

type MQTTHandler struct {
	db *sql.DB
}

func NewMQTTHandler(db *sql.DB) *MQTTHandler {
	return &MQTTHandler{db}
}

func (mh *MQTTHandler) HandleMetricEnviro(client mqtt.Client, message mqtt.Message) {
	log.Printf("Handling topic %#v message: %#v", message.Topic(), message.Payload())
	var data enviro_proto.EnvironmentalData
	if err := proto.Unmarshal(message.Payload(), &data); err != nil {
		log.Printf("Error unmarshalling EnvironmentalData message: %v", err)
		return
	}

	text, err := prototext.Marshal(&data)
	if err != nil {
		log.Printf("Error text formatting EnvironmentalData message: %v", err)
		return
	}
	log.Printf("Unmarshalled to: %s", text)

	insertQuery := "INSERT INTO enviro (timestamp, aqi_pm25_standard_5m_avg, temp_c_5m_avg, humidity_5m_avg) VALUES ($1, $2, $3, $4)"
	_, err = mh.db.Exec(insertQuery, time.Unix(int64(data.GetTimestamp()), 0), data.GetAqiPm25Standard_5MAvg(), data.GetTempC_5MAvg(), data.GetHumidity_5MAvg())
	if err != nil {
		log.Printf("Error inserting data into postgres: %v", err)
		return
	}

	message.Ack()
}

func (mh *MQTTHandler) HandleStateVan(client mqtt.Client, message mqtt.Message) {
	log.Printf("Handling topic %#v message: %#v", message.Topic(), message.Payload())
	var data van_state_proto.VanState
	if err := proto.Unmarshal(message.Payload(), &data); err != nil {
		log.Printf("Error unmarshalling VanState message: %v", err)
		return
	}

	text, err := prototext.Marshal(&data)
	if err != nil {
		log.Printf("Error text formatting VanState message: %v", err)
		return
	}
	log.Printf("Unmarshalled to: %s", text)

	var sqlEngineState string
	switch data.GetEngineState() {
	case van_state_proto.VanState_ENGINE_OFF:
		sqlEngineState = "OFF"
	case van_state_proto.VanState_ENGINE_RUNNING:
		sqlEngineState = "RUNNING"
	default:
		log.Printf("Unexpected engine state: %s", data.GetEngineState().String())
		return
	}

	insertQuery := "INSERT INTO van_state (timestamp, engine_state) VALUES ($1, $2)"
	_, err = mh.db.Exec(insertQuery, time.Now(), sqlEngineState)
	if err != nil {
		log.Printf("Error inserting data into postgres: %v", err)
		return
	}

	message.Ack()
}
