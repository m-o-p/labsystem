<?php
// alert/ info data may come via url or via set $SYS{INFO, ALERT}
if ($url->available('sysinfo')) {
  if (isset($SYSINFO)) {
    $SYSINFO .= htmlentities(html_entity_decode($url->get('sysinfo')));
  } else {
    $SYSINFO = htmlentities(html_entity_decode($url->get('sysinfo')));
  }
}
if ($url->available('sysalert')) {
  if (isset($SYSALERT)) {
    $SYSALERT .= htmlentities(html_entity_decode($url->get('sysalert')));
  } else {
    $SYSALERT = htmlentities(html_entity_decode($url->get('sysalert')));
  }
}