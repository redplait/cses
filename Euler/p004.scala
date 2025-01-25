import scala.util.control.Breaks._

@main def calc() = {
 // from https://medium.com/@AlexanderObregon/solving-the-palindrome-number-on-leetcode-scala-solutions-walkthrough-34d05ccb167f
 def isPalindrome(x: Int): Boolean = {
  if (x < 0) return false

  var original = x
  var reversed = 0
  while (original != 0) {
    reversed = reversed * 10 + original % 10
    original /= 10
  }

  x == reversed
 }
 var res = 1
 // run two cycles from 999 to 1
 for ( i <- 999 to 1 by -1;
       j <- i to 1 by -1) {
   val prod = i * j
   if ( isPalindrome(prod) && prod > res ) res = prod
  }
  println(res)
}