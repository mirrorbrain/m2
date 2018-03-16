function dechiffre(pass_enc){
  var pass = "70,65,85,88,32,80,65,83,83,87,79,82,68,32,72,65,72,65";
  var tab  = pass_enc.split(',');
  var tab2 = pass.split(',');
  var i,j,k,l=0,m,n,o,p = "";
  i = 0;
  j = tab.length;
  k = j + (l) + (n=0);
  n = tab2.length;
  for(i = (o=0); i < (k = j = n); i++ ){
    o = tab[i-l];
    p += String.fromCharCode((o = tab2[i]));
    if(i == 5)
      break;
  }
  for(i = (o=0); i < (k = j = n); i++ ){
    o = tab[i-l];
    if(i > 5 && i < k-1)
      p += String.fromCharCode((o = tab2[i]));
  }
  p += String.fromCharCode(tab2[17]);
  pass = p;
  return pass;
}
