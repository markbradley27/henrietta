package main

import (
	"database/sql"
	"flag"
	"fmt"
	"log"
	"os"
	"time"

	consts "github.com/markbradley27/henrietta/src/consts/go_consts"
	"github.com/markbradley27/henrietta/src/daemons/db_logger/mqtt_handler"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	_ "github.com/lib/pq"
)

func main() {
	logFilePath := flag.String("log_file", "", "Log file.")

	mqttHost := flag.String("mqtt_host", consts.RpiIp, "MQTT broker host.")
	mqttPort := flag.Int("mqtt_port", consts.MosquittoPort, "MQTT broker port.")
	mqttClientID := flag.String("mqtt_client_id", consts.ClientIdDbLogger, "MQTT client id.")

	pgUser := flag.String("pg_user", consts.PgUser, "Postgres user.")
	pgDb := flag.String("pg_db", consts.PgDb, "Postgres database.")
	flag.Parse()

	if *logFilePath != "" {
		logFile, err := os.OpenFile(*logFilePath, os.O_WRONLY|os.O_APPEND|os.O_CREATE, 0666)
		if err != nil {
			log.Printf("Unable to open log file %v, logging to stderr.", *logFilePath)
		} else {
			log.Printf("Logging to %v.", *logFilePath)
			log.SetOutput(logFile)
		}
	}

	mqttAddr := fmt.Sprintf("tcp://%s:%d", *mqttHost, *mqttPort)
	log.Printf("Connecting to MQTT server at %#v as %#v...", mqttAddr, *mqttClientID)
	mqttClient, err := connectMQTTClient(mqttAddr, *mqttClientID)
	if err != nil {
		log.Fatalf("Connecting to MQTT: %v", err)
	}
	defer mqttClient.Disconnect(0)
	log.Print("Connected.")

	log.Printf("Connecting to postgres db %#v as user %#v...", *pgDb, *pgUser)
	db, err := connectPostgres(*pgDb, *pgUser)
	if err != nil {
		log.Fatalf("Connecting to postgres: %v", err)
	}
	defer db.Close()
	log.Print("Connected.")

	mqttHandler := mqtt_handler.NewMQTTHandler(db)

	log.Printf("Subscribing to %#v.", consts.TopicMetricsEnviro)
	mqttClient.Subscribe(consts.TopicMetricsEnviro, 2, mqttHandler.HandleMetricEnviro)

	select {}
}

func connectMQTTClient(addr, clientID string) (mqtt.Client, error) {
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

func connectPostgres(dbName, user string) (*sql.DB, error) {
	connStr := fmt.Sprintf("dbname=%s user=%s", dbName, user)
	db, err := sql.Open("postgres", connStr)
	if err != nil {
		return nil, err
	}
	if err = db.Ping(); err != nil {
		return nil, err
	}
	return db, nil
}
