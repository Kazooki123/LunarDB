package lunardb.cache

import scala.collection.mutable

class CacheManager {
  private val nodes: mutable.Map[String, CacheNode] = mutable.Map.empty

  def addNode(nodeId: String): CacheNode = {
    val node = new CacheNode(nodeId)
    nodes.put(nodeId, node)
    node
  }

  def removeNode(nodeId: String): Boolean = {
    nodes.remove(nodeId).isDefined
  }

  def getNode(nodeId: String): Option[CacheNode] = {
    nodes.get(nodeId)
  }

  def listNodes: Iterable[String] = {
    nodes.keys
  }
}
