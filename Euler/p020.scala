@main def calc() = {
// from https://www.baeldung.com/scala/find-factorial
 def factorialUsingReduce(num: Int): BigInt = {
  require(num >= 0, "Factorial is not defined for negative numbers")
  num match {
    case 0 => BigInt(1)
    case _ => (BigInt(1) to BigInt(num)).reduce(_ * _)
  }
 }
 println(factorialUsingReduce(100).toString.split("").filter(_ != "").map(_.toInt).sum)
}