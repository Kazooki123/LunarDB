package v1

import (
	"net/http"
	// "strconv"

	"github.com/gin-gonic/gin"
	"github.com/Kazooki123/lunardb/cache"
)

func SetupRoutes(router *gin.Engine, c *cache.Cache) {
	v1 := router.Group("/api/v1")
	{
		v1.GET("/get/:key", getHandler(c))
		v1.POST("/set", setHandler(c))
		// v1.DELETE("/del/:key", delHandler(c))
		// v1.POST("/mset", msetHandler(c))
		// v1.POST("/mget", mgetHandler(c))
		// v1.GET("/keys", keysHandler(c))
		// v1.POST("/clear", clearHandler(c))
		// v1.GET("/size", sizeHandler(c))
		// v1.POST("/cleanup", cleanupHandler(c))

		// List of operations
		v1.POST("/lpush", lpushHandler(c))
		// v1.POST("/lpop", lpopHandler(c))
		// v1.POST("/rpush", rpushHandler(c))
		// v1.POST("/rpop", rpopHandler(c))
		// v1.GET("/lrange", lrangeHandler(c))
		// v1.GET("/llen", llenHandler(c))
	}
}

func getHandler(c *cache.Cache) gin.HandlerFunc {
	return func(ctx *gin.Context) {
		key := ctx.Param("key")
		value, exists := c.Get(key)
		if !exists {
			ctx.JSON(http.StatusNotFound, gin.H{"error": "Key not found"})
			return
		}
		ctx.JSON(http.StatusOK, gin.H{"value": value})
	}
}

func setHandler(c *cache.Cache) gin.HandlerFunc {
	return func(ctx *gin.Context) {
		var req struct {
			Key   string `json:"key" binding:"required"`
			Value string `json:"value" binding:"required"`
			TTL   int    `json:"ttl"`
		}
		if err := ctx.ShouldBindJSON(&req); err != nil {
			ctx.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
			return
		}
		c.Set(req.Key, req.Value, req.TTL)
		ctx.JSON(http.StatusOK, gin.H{"message": "OK"})
	}
}

// Implementing other handlers (del, mset, mget, keys, clear, size, cleanup) similarly later on...

func lpushHandler(c *cache.Cache) gin.HandlerFunc {
	return func(ctx *gin.Context) {
		var req struct {
			Key   string `json:"key" binding:"required"`
			Value string `json:"value" binding:"required"`
		}
		if err := ctx.ShouldBindJSON(&req); err != nil {
			ctx.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
			return
		}
		c.LPush(req.Key, req.Value)
		ctx.JSON(http.StatusOK, gin.H{"message": "OK"})
	}
}

// Implementig other list operation handlers (lpop, rpush, rpop, lrange, llen) similarly later on...