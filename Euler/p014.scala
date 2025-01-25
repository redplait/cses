@main def calc() = {
 var kach = collection.mutable.Map.empty[Long, Int]
 def recColl(cum: Int, l: Long): Int = {
   if ( l == 1 ) cum
   else {
     kach.get(l) match {
       case Some(v) => cum + v
       case None => val res = if ( l %2 == 0 ) recColl(1, l / 2) else recColl(1, 3 * l + 1); kach.addOne(l, res); res + cum
     }
   }
 }
 var v = 1
 var idx = 1L
 for ( i <- 2 to 999999 ) recColl(0, i)
 kach.foreach( pair => if ( pair._2 > v ) { v = pair._2; idx = pair._1 } );
 println(idx)
}