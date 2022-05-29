package mqtt_handler

import (
	"database/sql"
	"log"
	"time"

	enviro_proto "github.com/markbradley27/henrietta/src/proto/enviro_go_proto"

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

	// TODO: Use the proto's timestamp field once enviro_micky knows what time it is.
	insertQuery := "INSERT INTO enviro (timestamp, aqi_pm25_standard_5_m_avg, temp_c_5_m_avg, humidity_5_m_avg) VALUES ($1, $2, $3, $4)"
	_, err = mh.db.Exec(insertQuery, time.Now(), data.GetAqiPm25Standard_5MAvg(), data.GetTempC_5MAvg(), data.GetHumidity_5MAvg())
	if err != nil {
		log.Printf("Error inserting data into postgres: %v", err)
		return
	}

	message.Ack()
}
