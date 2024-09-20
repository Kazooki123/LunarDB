package handlers

import (
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/Kazooki123/lunardb/apis/internal/database"
)

func SetupRoutes(router *gin.Engine, cache *database.Cache) {
	v1 := router.Group("/api/v1")
	{
		v1.GET("/get/:key", getHandler(cache))
		v1.POST("/set", setHandler(cache))
		// v1.DELETE("/del/:key", delHandler(cache))
		// v1.POST("/mset", msetHandler(cache))
		// v1.POST("/mget", mgetHandler(cache))
		// v1.GET("/keys", keysHandler(cache))
		// v1.POST("/clear", clearHandler(cache))
		// v1.GET("/size", sizeHandler(cache))
		// v1.POST("/cleanup", cleanupHandler(cache))
		
		// List operations
		// v1.POST("/lpush", lpushHandler(cache))
		// v1.POST("/lpop", lpopHandler(cache))
		// v1.POST("/rpush", rpushHandler(cache))
		// v1.POST("/rpop", rpopHandler(cache))
		// v1.GET("/lrange", lrangeHandler(cache))
		// v1.GET("/llen", llenHandler(cache))
	}
}

func getHandler(cache *database.Cache) gin.HandlerFunc {
	return func(ctx *gin.Context) {
		key := ctx.Param("key")
		value, exists := cache.Get(key)
		if !exists {
			ctx.JSON(http.StatusNotFound, gin.H{"error": "Key not found"})
			return
		}
		ctx.JSON(http.StatusOK, gin.H{"value": value})
	}
}

func setHandler(cache *database.Cache) gin.HandlerFunc {
	return func(ctx *gin.Context) {
		var req struct {
			Key   string      `json:"key" binding:"required"`
			Value interface{} `json:"value" binding:"required"`
			TTL   int         `json:"ttl"`
		}
		if err := ctx.ShouldBindJSON(&req); err != nil {
			ctx.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
			return
		}
		cache.Set(req.Key, req.Value, req.TTL)
		ctx.JSON(http.StatusOK, gin.H{"message": "OK"})
	}
}
