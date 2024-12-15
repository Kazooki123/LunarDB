package lunardb.cache

import scala.collection.mutable

class CacheNode(val nodeId: String) {
  private val cache: mutable.Map[String, String] = mutable.Map.empty

  def put(key: String, value: String): Unit = {
    cache.put(key, value)
  }

  def get(key: String): Option[String] = {
    cache.get(key)
  }

  def delete(key: String): Boolean = {
    cache.remove(key).isDefined
  }

  def keys: Iterable[String] = {
    cache.keys
  }

  override def toString: String = "CacheNode($nodeId)"
}
