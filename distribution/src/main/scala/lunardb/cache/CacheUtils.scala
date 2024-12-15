package lunardb.cache

import java.nio.charset.StandardCharsets
import java.security.MessageDigest
import scala.util.{Try, Failure, Success}

object CacheUtils {

  /**
    * Hashes a given key using SHA-256 for consistent hashing.
    *
    * @param key The key to hash.
    * @return The hashed value as a hexadecimal string.
    */
  def hashKey(key: String): String = {
    val digest = MessageDigest.getInstance("SHA-256")
    val hashBytes = digest.digest(key.getBytes(StandardCharsets.UTF_8))
    hashBytes.map("%02x".format(_)).mkString
  }

  /**
    * Serialize a data object to a string representation.
    *
    * @param data The data to serialize.
    * @return The serialized string.
    */
  def serialize(data: Any): String = {
    Try(data.toString) match {
      case Success(serializedData) => serializedData
      case Failure(exception) => throw new RuntimeException("Serialization failed!", exception)
    }
  }

  /**
    * Deserializes a string representation back into the original data.
    *
    * @param serializedData The string to deserialize
    * @param clazz The class type of deserialize into.
    * @tparam T The type of the object.
    * @return The deserialized object.
    */
  def deserialize[T](serializedData: String, clazz: Class[T]): T = {
    Try(clazz.getConstructor(classOf[String]).newInstance(serializedData)) match {
      case Success(deserializedData) => deserializedData
      case Failure(exception) =>throw new RuntimeException("Deserialization failed!", exception)
    }
  }

  /**
    * Converts a node identifier into a consistent hash.
    *
    * @param nodeId The node identifier.
    * @return The consistent hash for the node.
    */
  def generateNodeHash(nodeId: String): Int = {
    hashKey(nodeId).hashCode
  }

}
