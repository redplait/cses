import scala.io.Source

@main def calc() = {
 def worth(s: String): Int = {
   s.toArray.filter(_ != '"').map( _ - 'A' + 1).sum
 }
 // read names
 val source = Source.fromFile("0022_names.txt")
 val names = source.mkString
 source.close
 // elements if tuple (name, index in original file)
 val indexed = names.split(",").sortWith(_ < _).zipWithIndex
// println( indexed(937) )
 println( indexed.map( x => worth(x._1) * (1 + x._2)).sum )
}
