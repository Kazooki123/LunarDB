package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"strconv"
	"strings"

	"github.com/gorilla/mux"
)

// Mock Cache struct and methods (you'll need to implement actual connection to your C++ LunarDB)
type Cache struct{}

func (c *Cache) Get(key string) string {
	// Implement actual get logic
	return "mocked value"
}

func (c *Cache) Set(key, value string, ttl int) {
	// Implement actual set logic
}

func (c *Cache) Del(key string) bool {
	// Implement actual delete logic
	return true
}

func (c *Cache) Keys() []string {
	// Implementing actual logic later on
	// For now this is what I can handle
	// Feel free to contribute
	return []string{"MOCK_KEY01", "MOCK_KEY02"}
}

var cache = &Cache{}

func main() {
	r := mux.NewRouter()

	r.HandleFunc("/get/{key}", getHandler).Methods("GET")
	r.HandleFunc("/set", setHandler).Methods("POST")
	r.HandleFunc("/del/{key}", delHandler).Methods("DELETE")
	r.HandleFunc("/keys", keysHandler).Methods("GET")

	fmt.Println("Starting LunarDB API server on :8080")
	log.Fatal(http.ListenAndServe(":8080", r))
}

func getHandler(w http.ResponseWriter, r *http.Request) {
	vars := mux.Vars(r)
	key := vars["key"]
	value := cache.Get(key)
	if value == "" {
		http.Error(w, "Key not found", http.StatusNotFound)
		return
	}
	json.NewEncoder(w).Encode(map[string]string{"key": key, "value": value})
}

func setHandler(w http.ResponseWriter, r *http.Request) {
	var data map[string]string
	if err := json.NewDecoder(r.Body).Decode(&data); err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}
	key := data["key"]
	value := data["value"]
	ttl, _ := strconv.Atoi(data["ttl"])
	cache.Set(key, value, ttl)
	w.WriteHeader(http.StatusOK)
	fmt.Fprint(w, "OK")
}

func delHandler(w http.ResponseWriter, r *http.Request) {
	vars := mux.Vars(r)
	key := vars["key"]
	if cache.Del(key) {
		w.WriteHeader(http.StatusOK)
		fmt.Fprint(w, "OK")
	} else {
		http.Error(w, "Key not found", http.StatusNotFound)
	}
}

func keysHandler(w http.ResponseWriter, r *http.Request) {
	keys := cache.Keys()
	json.NewEncoder(w).Encode(map[string][]string{"keys": keys})
}
