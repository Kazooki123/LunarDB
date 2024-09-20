package database

import (
	"sync"
)

type Cache struct {
	data     map[string]interface{}
	maxSize  int
	mu       sync.RWMutex
}

func NewCache(maxSize int) *Cache {
	return &Cache{
		data:    make(map[string]interface{}),
		maxSize: maxSize,
	}
}

func (c *Cache) Set(key string, value interface{}, ttl int) {
	c.mu.Lock()
	defer c.mu.Unlock()

	if len(c.data) >= c.maxSize {
		c.evict()
	}

	c.data[key] = value
}

func (c *Cache) Get(key string) (interface{}, bool) {
	c.mu.RLock()
	defer c.mu.RUnlock()

	value, exists := c.data[key]
	return value, exists
}

func (c *Cache) Del(key string) bool {
	c.mu.Lock()
	defer c.mu.Unlock()

	_, exists := c.data[key]
	if exists {
		delete(c.data, key)
	}
	return exists
}

func (c *Cache) evict() {
	// Hold for now
}
