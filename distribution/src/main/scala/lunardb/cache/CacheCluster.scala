package lunardb.cache

import lunardb.cache.CacheUtils
import lunardb.cache.CacheManager
import lunardb.cache.CacheNode

class CacheCluster {
  private val manager = new CacheManager

  def addNode(nodeId: String): Unit = {
    manager.addNode(nodeId)
  }

  def put(key: String, value: String): Unit = {
    val nodeId = CacheUtils.hashKey(key, manager.listNodes.toSeq)
    manager.getNode(nodeId).foreach(_.put(key, value))
  }

  def get(key: String): Option[String] = {
    val nodeId = CacheUtils.hashKey(key, manager.listNodes.toSeq)
    manager.getNode(nodeId).flatMap(_.get(key))
  }

  def delete(key: String): Boolean = {
    val nodeId = CacheUtils.hashKey(key, manager.listNodes.toSeq)
    manager.getNode(nodeId).exists(_.delete(key))
  }
}
