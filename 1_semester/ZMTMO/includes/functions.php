<?php 

	function getpages(){
		$texts = file_get_contents("includes/texts.json");
		$texts = json_decode($texts);
		return $texts;
	}


	function getpage($page, $lang){
		foreach (getpages() as $text) {
			if( $page == $text->page ){
				if ($lang=="sk") {
					$title = $text->nadpis_sk;
					$content = $text->text_sk;
					$maintitle = $text->nazov_stranky_sk;
					$contact = $text->kontakt_sk;	
				}
				else{
					$title = $text->nadpis_en;
					$content = $text->text_en;
					$maintitle = $text->nazov_stranky_en;		
					$contact = $text->kontakt_en;
				}
				
				return (object)["title" => $title, "content" => $content,"maintitle" => $maintitle, "contact" => $contact];	
			}
		}		
		return 0;
	}
?>