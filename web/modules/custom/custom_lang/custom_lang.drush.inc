<?php

/**
 * @file
 * Contains the code to generate the custom drush commands.
 */

/**
 * Implements hook_drush_command().
 */
function custom_lang_drush_command() {
  $items['change-language'] = [
    'description' => 'Change the default language of the site.',
    'arguments' => [
      'lang_code' => 'The new default language code.',
      'default_domain' => 'The new domain for the default language.',
    ],
    'drupal dependencies' => ['custom_lang'],
    'aliases' => ['change:lang'],
  ];

  return $items;
}

/**
 * Call back for "change-language" drush command.
 */
function drush_custom_lang_change_language($lang_code, $default_domain) {
  if (empty($lang_code) || empty($default_domain)) {
    drush_print('Provide new default language code and domain.');
    return;
  }

  $configFactory = \Drupal::configFactory();

  // Set default language.
  $configFactory->getEditable('system.site')->set('default_langcode', $lang_code)->save();

  // Assign the multidev domain to the new language,
  // and give English domain a different domain.
  $config = $configFactory->getEditable('language.negotiation');
  $domains = $config->get('url.domains');
  $domains[$lang_code] = $default_domain;
  $domains['en'] = 'en.' . $default_domain;
  $config->set('url.domains', $domains);

  // English isn't default anymore, so English needs a prefix.
  $prefixes = $config->get('url.prefixes');
  $prefixes['en'] = 'en';
  $config->set('url.prefixes', $prefixes);

  $config->save(TRUE);

  drush_print('Changed the default language to: ' . $lang_code);
}
