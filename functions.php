<?php


function set_posts()
{
    global $post_list;
    if (!$post_list) $post_list = array();
    if ($handle = opendir('./data/posts')) {
        while (false !== ($entry = readdir($handle))) {
            array_push($post_list, $entry);
        }
        closedir($handle);
    }
}

function set_post()
{
    global $post_list, $post, $post_id;
    if (!$post_list) return;
    if (!$post_id) $post_id = 2;
    $filename = './data/posts/' . $post_list[$post_id];
    $post = implode(file($filename));
    $post_id++;
    return $post;
}


function has_post()
{
    global $post_list, $post_id;
    if (!$post_list) return;
    if (!$post_id) $post_id = 2;
    return $post_id < count($post_list);
}


