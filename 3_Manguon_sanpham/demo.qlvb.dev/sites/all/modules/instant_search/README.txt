
Description
-----------

By default, Drupal indexes node content only when running the cron. This means
that your new or updated content may only appear in search results after a few
minutes, when cron has run. Using Instant search, you can make Drupal index
your new nodes when submitting them (inserting or updating), after which they
will instantly appear in search results.


Requirements
------------

This module requires Drupal 6.


Installation
------------

1) Copy/upload the instant_search module folder to the sites/all/modules
directory of your Drupal installation. 

2) Enable the instant search module in Drupal (administer -> modules).


Configuration
-------------

You can enable/disable Instant search temporarily on the search administration
page.

Instant search can be configured at : 
  Administer -> Site configuration -> Search


Author
------

Davy Van Den Bremt <info@davyvandenbremt.be>
http://www.davyvandenbremt.be