@main def calc() = {
 val coins = Array[Int](1, 2, 5, 10, 20, 50, 100, 200)
 val total = 200
 // I hope Long is enough
 val dp = Array.ofDim[Long](coins.size + 1, total + 1)
 dp(0)(0) = 1
 for ( i <- (0 until coins.size) ) {
   val coin = coins(i)
   for ( j <- (0 to total) ) dp(i + 1)(j) = dp(i)(j) + ( if (j >= coin) then dp(i + 1)(j - coin) else 0 )
 }
 println(dp(coins.size)(total))
}