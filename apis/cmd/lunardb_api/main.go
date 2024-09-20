package main

import (
	"fmt"
	"log"
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/Kazooki123/lunardb/apis/internal/database"
	"github.com/Kazooki123/lunardb/apis/internal/handlers"
	"github.com/Kazooki123/lunardb/apis/internal/middleware"
)

func main() {
	// Initialize the cache
	cache := database.NewCache(1000) // Create a cache with a maximum of 1000 entries

	// Set up the Gin router
	router := gin.Default()

	// Apply middleware
	router.Use(middleware.Logger())

	// Initialize the API routes
	handlers.SetupRoutes(router, cache)

	// Start the server
	fmt.Println("Starting LunarDB API server on :8080")
	log.Fatal(http.ListenAndServe(":8080", router))
}