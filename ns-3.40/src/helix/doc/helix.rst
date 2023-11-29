Example Module Documentation
----------------------------

.. include:: replace.txt
.. highlight:: cpp

.. heading hierarchy:
   ------------- Chapter
   ************* Section (#.#)
   ============= Subsection (#.#.#)
   ############# Paragraph (no number)

This is a suggested outline for adding new module documentation to |ns3|.
See ``src/click/doc/click.rst`` for an example.

The introductory paragraph is for describing what this code is trying to
model.

For consistency (italicized formatting), please use |ns3| to refer to
ns-3 in the documentation (and likewise, |ns2| for ns-2).  These macros
are defined in the file ``replace.txt``.

HELIX is a level 4 transport layer protocol, originally designed for
FPGAs for very high rates of data transfer. You can read the paper
here: (TODO: add link).

This module is an |ns3| implementation of this protocol. Specifically,
it interfaces with a preexisting Rust implementation of this protocol.

Model Description
*****************

The source code for the new module lives in the directory ``src/helix``.

This is a model of the HELIX transport layer protocol.

Design
======

This module interfaces with a Rust implementation of HELIX. This model
follows the normal |ns3| design conventions for transport layer protocols
with one exception. Instead of the main body of the protocol living in the
``src/helix/model/helix-socket-impl.cc``, it is in the helix-rs Rust library.

The file ``src/helix/model/helix-rs-interface.cc`` is the interface between
|ns3| and the Rust implementation. It handles all type conversion and invocations
of Rust functions. All information between these two modules is passed as a 
FFISharedBuffer struct. Likewise, the Rust implementation has a similar
set of functions designated for type conversion before actually passing any
data to or from the Rust Helix implementation.

The file ``src/helix/model/helix-rs-interface.cc`` interfaces with the
``src/helix/model/helix-socket-impl.cc`` file which would traditionally be
meant for holding the implementation.

HELIX is built ontop of UPD, but it still acts and is meant to be used as
a transport layer protcol. For this reason, the helix-socket-impl object
holds an instance of a UDP socket which is used to integrate direclty
with |ns3|.

The following is a diagram to explain how HELIX fits into the |ns3| stack.
``Diagram:
            +-----------------------+
            |         ...           |
            |        (ns3)          |
            +-----------------------+
                  |        ^  ...
                  v        |
            +-----------------------+
            |     application       |
            |     layer (ns3)       |
            +-----------------------+
                  |        ^  Packet (ns3)
                  |        |
                  |        |
                  v        |
            +-----------------------+
            |     HELIX Socket      |
            |  Implementation (ns3) |
            +-----------------------+
                  |        ^  FFI Helix Packet
                  v        |
            +-----------------------+
            |     HELIX Protocol    |
            | Implementaiton (Rust) |
            +-----------------------+
                  |        ^  FFI Helix Packet
                  v        |
            +-----------------------+
            |     HELIX Socket      |
            |  Implementation (ns3) |
            +-----------------------+
                  |        ^  Packets  (ns3)
                  |        |
                  |        |
                  v        |
            +-----------------------+
            |          UDP          |
            | Socket Interface (ns3)|
            +-----------------------+
                  |        ^  ...
                  v        |
            +-----------------------+
            |         ...           |
            |        (ns3)          |
            +-----------------------+
``

Scope and Limitations
=====================

What can the model do?  What can it not do?  Please use this section to
describe the scope and limitations of the model.

References
==========

Add academic citations here, such as if you published a paper on this
model, or if readers should read a particular specification or other work.

TODO: cite Leon's paper

Usage
*****

This section is principally concerned with the usage of your model, using
the public API.  Focus first on most common usage patterns, then go
into more advanced topics.

Building New Module
===================

Include this subsection only if there are special build instructions or
platform limitations.

Prior to building, there are two edits that need to be made to 
``ns-3.40/src/network/model/socket.h``. Both ``SetSendCallback``
and ``SetRecvCallback`` need to be marked as virtual functions.

Helpers
=======

What helper API will users typically use?  Describe it here.

Currently, there is one helper function called ``HelixStackHelper::AddHelix()``.
It is responsible for installing the HELIX protocol on a given node, and must
be used in order to use HELIX with a node.

Attributes
==========

What classes hold attributes, and what are the key ones worth mentioning?

Output
======

What kind of data does the model generate?  What are the key trace
sources?   What kind of logging output can be enabled?

Advanced Usage
==============

Go into further details (such as using the API outside of the helpers)
in additional sections, as needed.

Examples
========

What examples using this new code are available?  Describe them here.

See ``ns-3.40/src/helix/examples/helix-example.cc`` and see
``ns-3.40/src/helix/examples/helix-large-transfer.cc`` for usage examples.

Troubleshooting
===============

Add any tips for avoiding pitfalls, etc.

Validation
**********

Describe how the model has been tested/validated.  What tests run in the
test suite?  How much API and code is covered by the tests?  Again,
references to outside published work may help here.
