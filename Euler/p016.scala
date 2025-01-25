@main def calc() = {
  // not worse than perl one-liner, he-he
  println( BigInt("2").pow(1000).toString.split("").filter(_ != "").map(_.toInt).sum )
}