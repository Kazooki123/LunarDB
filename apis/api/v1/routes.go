package main

import (
	"github.com/gin-gonic/gin"
)

func setRoutes(r *gin.Engine) {
	v1 := r.Group("/api/v1")
	{
		v1.POST("/execute")
		// Add more routes here as your API grows
	}
}
