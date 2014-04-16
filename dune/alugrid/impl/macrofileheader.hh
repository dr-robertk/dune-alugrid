#ifndef DUNE_ALUGRID_IMPL_MACROFILEHEADER_HH
#define DUNE_ALUGRID_IMPL_MACROFILEHEADER_HH

#include <cstdlib>
#include <iostream>

#include <dune/alugrid/impl/binaryio.hh>

namespace ALUGrid
{

  // MacroFileHeader
  // ---------------

  struct MacroFileHeader
  {
    enum Type { tetrahedra, hexahedra };
    enum Format { ascii, binary, zbinary };
    enum ByteOrder { native, bigendian, littleendian };

    static std::string toString ( Type type ) { return stringType[ type ]; }
    static std::string toString ( Format format ) { return stringFormat[ format ]; }
    static std::string toString ( ByteOrder byteOrder ) { return stringByteOrder[ byteOrder ]; }

    MacroFileHeader ()
      : type_( hexahedra ), format_( ascii ), byteOrder_( native ), size_( 0 )
    {}

    MacroFileHeader ( Type type, Format format )
      : type_( type ), format_( format )
    {}

    explicit MacroFileHeader ( const std::string &firstLine, bool verbose = false )
      : type_( hexahedra ), format_( ascii ), byteOrder_( native ), size_( 0 )
    {
      read( firstLine, verbose );
    }

    explicit MacroFileHeader ( std::istream &in, bool verbose = false )
      : type_( hexahedra ), format_( ascii ), byteOrder_( native ), size_( 0 )
    {
      read( in, verbose );
    }

    Type type () const { return type_; }
    void setType ( Type type ) { type_ = type; }
    bool setType ( const std::string &type );

    Format format () const { return format_; }
    void setFormat ( Format format ) { format_ = format; }
    bool setFormat ( const std::string &format );

    ByteOrder byteOrder () const { return byteOrder_; }
    void setByteOrder ( ByteOrder byteOrder ) { byteOrder_ = byteOrder; }
    bool setByteOrder ( const std::string &byteOrder );
    void setSystemByteOrder ();

    std::size_t size () const { return size_; }
    void setSize ( std::size_t size ) { size_ = size; }

    bool isBinary () const { return (format() == binary) || (format() == zbinary); }

    BinaryFormat binaryFormat () const;

    bool read ( const std::string &firstLine, bool verbose = false );
    bool read ( std::istream &in, bool verbose = false );
    void write ( std::ostream &out ) const;

  private:
    static const char *stringType[ 2 ];
    static const char *stringFormat[ 3 ];
    static const char *stringByteOrder[ 3 ];

    Type type_;
    Format format_;
    ByteOrder byteOrder_;
    std::size_t size_;
  };



  // Auxilliary Functions for MacroFileHeader
  // ----------------------------------------

  inline std::istream &operator>> ( std::istream &in, MacroFileHeader &header )
  {
    header.read( in );
    return in;
  }

  inline std::ostream &operator<< ( std::ostream &out, const MacroFileHeader &header )
  {
    header.write( out );
    return out;
  }

} // namespace ALUGrid

#endif // #ifndef DUNE_ALUGRID_IMPL_MACROFILEHEADER_HH
