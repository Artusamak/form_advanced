<?php

/**
 * @file
 * Definition of views_handler_filter_node_type.
 */

/**
 * Filter by node type.
 *
 * @ingroup views_filter_handlers
 */
class views_handler_filter_extra_access extends views_handler_filter_in_operator {
  function query() {
    // Get the special products.
    $q = new EntityFieldQuery();
    $q->entityCondition('entity_type', 'commerce_product');
    $q->fieldCondition($this->options['flag_field'], 'value', 1);
    $results = $q->execute();

    $product_ids = array();
    foreach (reset($results) as $product) {
      $product_ids[] = (int)$product->product_id;
    }
    // Get the allowed products from the current user.
    $user_product_ids = array();
    $user = user_load($GLOBALS['user']->uid);
    if ($user->uid > 0) {
      // Fetch the ids from the current user.
      $products = field_get_items('user', $user, $this->options['user_authorized_products_field']);
      foreach ($products as $product) {
        $user_product_ids[] = (int)$product['target_id'];
      }
    }
    $exclude_ids = array_diff($product_ids, $user_product_ids);
    if (count($exclude_ids) > 0) {
      $this->query->add_where(2, $this->options['node_products_table'] . '.' . $this->options['node_products_column'], $exclude_ids, 'NOT IN');
    }
  }

  function options_form(&$form, &$form_state) {
    foreach (field_read_instances(array('entity_type' => 'commerce_product')) as $option) {
      $options[$option['field_name']] = $option['label'];
    }
    $form['flag_field'] = array(
      '#type' => 'radios',
      '#options' => $options,
      '#title' => t('Flag field'),
      '#description' => t('Field used to flag a product with limited access. This list is based on the commerce product entity type.'),
      '#default_value' => $this->options['flag_field'],
    );

    $options = array();
    foreach (field_read_instances(array('entity_type' => 'user')) as $option) {
      $options[$option['field_name']] = $option['label'];
    }
    $form['user_authorized_products_field'] = array(
      '#type' => 'radios',
      '#options' => $options,
      '#title' => t('User authorized products field'),
      '#description' => t('Field used to list the products that the user can see. This list is based on the user entity type.'),
      '#default_value' => $this->options['user_authorized_products_field'],
    );
    $options = array();
    foreach (field_read_instances(array('entity_type' => 'node')) as $option) {
      $options[$option['field_name']] = $option['label'];
    }
    $form['node_products_field'] = array(
      '#type' => 'radios',
      '#options' => $options,
      '#title' => t('Product reference field'),
      '#description' => t('Field used to reference the products displayed. This list is based on the node entity type.'),
      '#default_value' => $this->options['node_products_field'],
    );
  }

  /**
   * Define "reduce" option.
   */
  public function option_definition() {
    $options['flag_field'] = array('default' => FALSE);
    $options['user_authorized_products_field'] = array('default' => FALSE);
    $options['node_products_field'] = array('default' => FALSE);
    $options['node_products_table'] = array('default' => FALSE);
    $options['node_products_column'] = array('default' => FALSE);
    return $options;
  }

  /**
   * Simple submit handler
   */
  function options_submit(&$form, &$form_state) {
    $field_name = $form_state['values']['options']['node_products_field'];
    $field = field_read_field($field_name);
    $form_state['values']['options']['node_products_table'] = _field_sql_storage_tablename($field);
    $form_state['values']['options']['node_products_column'] = $field_name . '_' . key($field['columns']);
  }
}
