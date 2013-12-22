<?php
/**
 * Encodes MIME/HTTP header values that contain non-ASCII, UTF-8 encoded
 * characters.
 *
 * For example, mime_header_encode('tést.txt') returns "=?UTF-8?B?dMOpc3QudHh0?=".
 *
 * See http://www.rfc-editor.org/rfc/rfc2047.txt for more information.
 *
 * Notes:
 * - Only encode strings that contain non-ASCII characters.
 * - We progressively cut-off a chunk with truncate_utf8(). This is to ensure
 *   each chunk starts and ends on a character boundary.
 * - Using \n as the chunk separator may cause problems on some systems and may
 *   have to be changed to \r\n or \r.
 */
function mime_header_encode($string) {
  if (preg_match('/[^\x20-\x7E]/', $string)) {
    $chunk_size = 47; // floor((75 - strlen("=?UTF-8?B??=")) * 0.75);
    $len = strlen($string);
    $output = '';
    while ($len > 0) {
      $chunk = truncate_bytes($string, $chunk_size);
      $output .= ' =?UTF-8?B?' . base64_encode($chunk) . "?=\n";
      $c = strlen($chunk);
      $string = substr($string, $c);
      $len -= $c;
    }
    return trim($output);
  }
  return $string;
}

function capsule_send_mail($body, $simplebody, $object, $recipient, $sender, $additional_headers = array()){
  $eol = "\n";
  $msg = "";

  $boundary = md5(date('r', time()));
  
  $headers = array(
    'From' => $sender,
    'Sender' => $sender,
    'Reply-To' => $sender,
    'MIME-Version' => '1.0',
    'Content-Type' => 'multipart/alternative; boundary="' . $boundary . '"'
  );
  array_merge($headers, $additional_headers);

  //  $msg .= "--".$boundary.$eol;
  //  $msg .= "Content-Type: multipart/alternative".$eol;
  //  $msg .= "boundary: \"$boundary\"".$eol.$eol;
  $msg .= "--".$boundary.$eol;
  $msg .= "Content-Type: text/plain; charset=\"utf-8\"".$eol;
  $msg .= "Content-Transfer-Encoding: base64".$eol.$eol;
  $msg .= chunk_split(base64_encode($simplebody)) . $eol.$eol;

  $msg .= "--".$boundary.$eol;
  $msg .= "Content-Type: text/html; charset=\"utf-8\"".$eol;
  $msg .= "Content-Transfer-Encoding: base64".$eol.$eol;
  $msg .= chunk_split(base64_encode($body)) . $eol.$eol;

  $msg .= "--".$boundary."--".$eol.$eol;
  
  $mimeheaders = array();
  foreach ($headers as $name => $value) {
    $mimeheaders[] = $name . ': ' . mime_header_encode($value);
  }
  mail($recipient, $object, $msg, join("\n", $mimeheaders));
}