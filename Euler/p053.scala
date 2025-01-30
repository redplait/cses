@main def calc() = {
  var res = 0
  for ( n <- (1 to 100); r <- (1 to n - 1) ) {
    var bCof = BigInt("1")
    for ( i <- (n - r + 1 to n) ) bCof *= i
    for ( j <- (2 to r) ) bCof /= j
    if ( bCof > 1000000 ) res += 1
  }
  println(res)
}