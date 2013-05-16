<?php

function sim_feature_views_data() {
  // Expose the created and changed timestamps.
  $data['commerce_product']['filter_access'] = array(
    'title' => t('Special filter access'),
    'help' => t('The date the profile was created.'),
    'filter' => array(
      'handler' => 'views_handler_filter_extra_access',
    ),
  );
  return $data;
}
