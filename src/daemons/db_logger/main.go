package main

import (
	"database/sql"
	"flag"
	"fmt"
	"log"

	consts "github.com/markbradley27/henrietta/src/consts/go_consts"
	"github.com/markbradley27/henrietta/src/daemons/db_logger/mqtt_handler"
	"github.com/markbradley27/henrietta/src/daemons/log_util"
	"github.com/markbradley27/henrietta/src/daemons/mqtt_util"

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

	log_util.SetupLogFile(*logFilePath)

	log.Printf("Connecting to MQTT server at %s:%d as %#v...", *mqttHost, *mqttPort, *mqttClientID)
	mqttClient, err := mqtt_util.ConnectMQTTClient(*mqttHost, *mqttPort, *mqttClientID)
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
