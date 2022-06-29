package log_util

import (
	"log"
	"os"
)

// If path is empty, logs to stderr.
func SetupLogFile(path string) {
	if path != "" {
		logFile, err := os.OpenFile(path, os.O_WRONLY|os.O_APPEND|os.O_CREATE, 0666)
		if err != nil {
			log.Printf("Unable to open log file %v, logging to stderr.", path)
		} else {
			log.Printf("Logging to %v.", path)
			log.SetOutput(logFile)
		}
	}
}
