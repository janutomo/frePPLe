===============
Modelling guide
===============

This chapter describe the frePPLe data entities, their fields and relationships.

The planning engine has 2 native APIs:

* | **XML**:
  | The files frepple.xsd and frepple_core.xsd define the XML Schema of the
    frePPLe data.
  | The XML-data can be placed in any namespace. To support subclassing the
    namespace xsi must be defined as “http://www.w3.org/2001/XMLSchema-instance”.
  | FrePPLe XML files thus typically have the following structure:

  ::

      <?xml version="1.0" encoding="UTF-8"?>
      <plan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
      ...
      </plan>

  | The following encodings are supported for XML data: ASCII, UTF-8, UTF-16
    (Big/Small Endian), UTF-32(Big/Small Endian), EBCDIC code pages IBM037,
    IBM1047 and IBM1140, ISO-8859-1 (aka Latin1) and Windows-1252. UTF-8 will
    be the best choice in most situations.

* | **Python**:
  | FrePPLe embeds an interpreter for the Python language. All objects in
    the planning engine can be read, created, updated and deleted from Python
    code. All functionality of Python and its extension modules is accessible
    from the planning engine.
  | Detailed programming and scripting is possible in this way. For complex
    integration tasks and for customization of the algorithms Python is your
    big friend.

Before diving into the details, have a look at the domain model diagrams.
They show clear and simple the main entities and their relationships.

.. toctree::
   :maxdepth: 2

   simplified-domain-model
   detailed-domain-model
   environment-variables
   python-interpreter
   global-parameters
   buffer
   calendar
   customer
   demand
   flow
   item
   load
   location
   operation
   suboperation
   operationplan
   problem
   resource
   resource-skill
   setup-matrix
   skill
   solver
