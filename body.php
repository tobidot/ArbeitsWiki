<body>

<?php
global $post_list;
set_posts();

while (has_post()) {
    echo "next";
    echo set_post();
}



?>

</body>
