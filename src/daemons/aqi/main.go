package main

import (
	"fmt"
	"io"
	"log"
	"net"
)

const (
	LISTEN_HOST = "192.168.138.1"
	LISTEN_PORT = 42901
)

func main() {
	listenAddr := fmt.Sprintf("%v:%v", LISTEN_HOST, LISTEN_PORT)
	ln, err := net.Listen("tcp", listenAddr)
	if err != nil {
		log.Fatal("Error listening:", err.Error())
	}
	defer ln.Close();
	log.Print("Listening on: ", listenAddr);

	for {
		conn, err := ln.Accept()
		if err != nil {
			log.Print("Error accepting:", err.Error())
			continue
		}
		defer conn.Close();
		log.Print("Accepted connection from: ", conn.RemoteAddr().String())

		buf := make([]byte, 1024)
		data := ""
		for {
			n, err := conn.Read(buf)
			if err != nil {
				if err != io.EOF {
					log.Print("Error reading:", err.Error())
				}
				break
			}
			data = data + string(buf[:n])
		}
		log.Print("Got data: ", data)
	}
}
