import scala.io.Source
import scala.collection.mutable.Set

@main def calc() = {
 def worth(s: String): Int = {
   s.toArray.filter(_ != '"').map( _ - 'A' + 1).sum
 }
 // read names
 val source = Source.fromFile("0042_words.txt")
 val names = source.mkString
 source.close
 // make triangles
 def getTri(idx: Int): Int = {
   idx * (idx + 1) / 2
 }
 val tri = (1 to 24).map( getTri(_) )
 val tris = Set[Int]( tri* )
 println( names.split(",").filter( x=> tris.contains( worth(x) ) ).size )
}