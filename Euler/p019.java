// I don't see much sence to manually re-implement over-complicated crazy calendar logic
// especially since java 8+ has LocalDate api (not perfect but acceptable)
// see details for example in book "Modern Java in Action", chapter 12
import java.time.DayOfWeek;
import java.time.LocalDate;

public class p019 {
 static int calc() {
   int res = 0;
   for ( var curr = LocalDate.of(1901, 1, 1); curr.getYear() < 2001; curr = curr.plusMonths(1) )
   {
     if ( curr.getDayOfWeek() == DayOfWeek.SUNDAY ) { res++; }
   }
   return res;
 }
 public static void main(String... args) {
   System.out.println(calc());
 }
}