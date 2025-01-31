@main def calc() = {
 // from https://stackoverflow.com/questions/10274191/palindromes-using-scala
 def isPalindrome(someNumber:String):Boolean = {
  val len = someNumber.length
  for(i <- 0 until len/2) {
    if(someNumber(i) != someNumber(len-i-1)) return false
  }
  true
 }
 def revAdd(n: BigInt): BigInt = {
  n + BigInt(n.toString.reverse.toString)
 }
 def isLych(n: Int): Boolean = {
   var cur:BigInt = n
   for ( i <- (1 until 50) ) {
     cur = revAdd(cur)
     if ( isPalindrome(cur.toString) ) return false
   }
   true
 }
 // main - check numbers up to 10000
 var res = 0
 for ( i <- (1 until 10000) if isLych(i) ) res += 1
 println(res)
}