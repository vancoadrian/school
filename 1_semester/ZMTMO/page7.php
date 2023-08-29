<?php include "includes/header.php" ?>

<?php 
  function clear($str){
    $str = trim($str);
    $str = htmlspecialchars($str);
    $str = filter_var($str,FILTER_SANITIZE_STRING);
    return $str;
  }

  if (isset($_POST['potvrdit'])) {
    $celemeno = clear($_POST['celemeno']);
    $sprava = clear($_POST['sprava']);
      $sprava = array(
        'celemeno' =>$celemeno,
        'sprava' =>$sprava
      );
      $komentare = file_get_contents("includes/comments.json");
      $komentare = json_decode($komentare);
      array_push($komentare, $sprava);
      $komentare = json_encode($komentare);
      file_put_contents("includes/comments.json", $komentare);
  }

 ?>


  <aside class="sideimage ib">
      <img src="img/phonemain.png" alt="">
      <img src="img/phoneback.png" alt="">
  </aside>

  <section class="mainarticle ib">

    <article class="allcomments">
      <?php 
        $komentare = file_get_contents("includes/comments.json");
        $komentare = json_decode($komentare);

        foreach ($komentare as $komentar) {
           echo '<article class="comment">';
           echo '<h3>'.$komentar->celemeno.'</h3>';
           echo '<p>'.$komentar->sprava.'</p>';
           echo '</article>';
        }

      ?>
    </article>
    
    <form action="#" method="post">
       <h1>Napíšte komentár</h1>
       <input type="text" placeholder="Meno a priezvisko" name="celemeno">
       <textarea name="sprava" id="comments" cols="30" rows="5"></textarea>
       <input type="submit" value="Potvrdiť" name="potvrdit">
    </form>
    
  </section>
  
<?php include "includes/footer.php" ?>