// Author: Kazooki123, Starloexolizzz

package main

import (
	"fmt"
	"log"
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/yourusername/lunardb/api/v1"
	"github.com/yourusername/lunardb/cache"
)

func main() {
	// Initializes the cache
	c := cache.NewCache(1000) // Creates a cache with a maximum of 1000 entries.

	// Set up the Gin router
	router := gin.Default()

	// Initialize the API routes
	v1.SetupRoutes(router, c)

	// Start the server
	fmt.Println("Starting LunarDB API server on :8080")
	log.Fatal(http.ListenAndServe(":8080", router))
}