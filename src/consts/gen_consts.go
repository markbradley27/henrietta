package main

import (
	"flag"
	"fmt"
	yaml "github.com/go-yaml/yaml"
	"io/ioutil"
	"log"
	"os"
	"strings"
)

func main() {
	yamlPath := flag.String("yaml_path", "./consts.yaml", "Path to consts yaml file.")
	cOutPath := flag.String("c_out", "", "Output c file.")
	goOutPath := flag.String("go_out", "", "Output go file.")
	flag.Parse()

	consts, err := loadYAML(*yamlPath)
	if err != nil {
		log.Fatal(err)
	}

	if *cOutPath != "" {
		if err = generateC(consts, *cOutPath); err != nil {
			log.Fatal(err)
		}
	}
	if *goOutPath != "" {
		if err = generateGo(consts, *goOutPath); err != nil {
			log.Fatal(err)
		}
	}
}

func loadYAML(yamlPath string) (map[string]interface{}, error) {
	file, err := os.Open(yamlPath)
	if err != nil {
		return nil, fmt.Errorf("opening consts.yaml: %w", err)
	}
	defer file.Close()

	constsYaml, err := ioutil.ReadAll(file)
	if err != nil {
		return nil, fmt.Errorf("reading consts.yaml: %w", err)
	}
	log.Printf("constsYaml:\n%s", string(constsYaml))

	consts := make(map[string]interface{})
	if err = yaml.Unmarshal(constsYaml, &consts); err != nil {
		log.Fatal("unmarshalling consts.yaml: ", err)
	}
	return consts, nil
}

func generateC(consts map[string]interface{}, outPath string) error {
	var builder strings.Builder
	builder.WriteString("#ifndef HENRIETTA_SRC_CONSTS_H_\n")
	builder.WriteString("#define HENRIETTA_SRC_CONSTS_H_\n")
	builder.WriteString("\n")
	for key, val := range consts {
		builder.WriteString(fmt.Sprintf("#define %s %v\n", strings.ToUpper(key), val))
	}
	builder.WriteString("\n")
	builder.WriteString("#endif // HENRIETTA_SRC_CONSTS_H_\n")

	file, err := os.OpenFile(outPath, os.O_RDWR|os.O_CREATE, 0644)
	if err != nil {
		return fmt.Errorf("opening %s: %w", outPath, err)
	}
	defer file.Close()

	file.WriteString(builder.String())
	return nil
}

func generateGo(consts map[string]interface{}, outPath string) error {
	var builder strings.Builder
	builder.WriteString("package consts\n")
	builder.WriteString("\n")
	builder.WriteString("const (\n")
	for key, val := range consts {
		words := strings.Split(key, "_")
		for i, word := range words {
			words[i] = strings.Title(word)
		}
		builder.WriteString(fmt.Sprintf("\t%s = %#v\n", strings.Join(words, ""), val))
	}
	builder.WriteString(")\n")

	file, err := os.OpenFile(outPath, os.O_RDWR|os.O_CREATE, 0644)
	if err != nil {
		return fmt.Errorf("opening %s: %w", outPath, err)
	}
	defer file.Close()

	file.WriteString(builder.String())
	return nil
}
