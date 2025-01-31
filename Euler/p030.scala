@main def calc() = {
 // 9 ^ 5 = 59049
 // 2 * 59049 = 118098 > 99
 // 3 * 59049 = 177147 > 999
 // 4 * 59049 = 236196 > 9999
 // 5 * 59049 = 295245 > 99999
 // 6 * 59049 = 354294 < 999999
 // so we can limit brute-force to 6 digit numbers
 def pow5(i: Int) = { i * i * i * i * i }
 def sum5(i: Int) = {
  i.toString.toArray.map(_ - '0').map(pow5(_)).sum
 }
 // actually 1 IS the solution
 println( (2 to 999999).map( x => { val p = sum5(x); if ( p == x ) p else 0 }).sum )
}