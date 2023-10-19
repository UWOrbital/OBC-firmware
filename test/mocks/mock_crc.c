// Copied from obc/reliance_edge/util/crc.c with RED function calls removed
#include "obc_crc.h"

#include "obc_errors.h"

#include <stdint.h>
#include <redconf.h>
#include <redmacs.h>

#define SUSPICIOUS_CRC_VALUE (0xBAADC0DEU)

#define CRC_BITWISE (0U)
#define CRC_SARWATE (1U)
#define CRC_SLICEBY8 (2U)

/* ALGORITHM DEFINITIONS */

#if REDCONF_CRC_ALGORITHM == CRC_BITWISE

/*  The following is representative of the polynomial accepted by CCITT 32-bit
    and in IEEE 802.3, Ethernet 2 specification.

    x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
       (reverse order)
    1110 1101 1011 1000 1000 0011 0010 0000 1
     (E)  (D)  (B)  (8)  (8)  (3)  (2)  (0)
*/
#define CCITT_32_POLYNOMIAL (0xEDB88320U)

/** @brief Compute a CRC32 for the given data buffer.

    For CCITT-32 compliance, the initial CRC must be set to 0.  To CRC multiple
    buffers, call this function with the previously returned CRC value.

    @param ulInitCrc32  Starting CRC value.
    @param pBuffer      Data buffer to calculate the CRC from.
    @param ulLength     Number of bytes of data in the given buffer.

    @return The updated CRC value.
*/
static uint32_t RedCrc32Update(uint32_t ulInitCrc32, const void *pBuffer, uint32_t ulLength) {
  uint32_t ulCrc32;

  if (pBuffer == NULL) {
    ulCrc32 = SUSPICIOUS_CRC_VALUE;
  } else {
    const uint8_t *pbBuffer = pBuffer;
    uint32_t ulIdx;

    ulCrc32 = ~ulInitCrc32;

    for (ulIdx = 0U; ulIdx < ulLength; ++ulIdx) {
      uint32_t ulBit;

      ulCrc32 ^= pbBuffer[ulIdx];

      /*  Branchless inner loop (greatly improves performance).
       */
      for (ulBit = 0U; ulBit < 8U; ulBit++) {
        ulCrc32 = ((ulCrc32 & 1U) * CCITT_32_POLYNOMIAL) ^ (ulCrc32 >> 1U);
      }
    }

    ulCrc32 = ~ulCrc32;
  }

  return ulCrc32;
}

#elif REDCONF_CRC_ALGORITHM == CRC_SARWATE

/** @brief Compute a CRC32 for the given data buffer.

    For CCITT-32 compliance, the initial CRC must be set to 0.  To CRC multiple
    buffers, call this function with the previously returned CRC value.

    @param ulInitCrc32  Starting CRC value.
    @param pBuffer      Data buffer to calculate the CRC from.
    @param ulLength     Number of bytes of data in the given buffer.

    @return The updated CRC value.
*/
static uint32_t RedCrc32Update(uint32_t ulInitCrc32, const void *pBuffer, uint32_t ulLength) {
  static const uint32_t aulCrc32Table[] = {
      0x00000000U, 0x77073096U, 0xEE0E612CU, 0x990951BAU, 0x076DC419U, 0x706AF48FU, 0xE963A535U, 0x9E6495A3U,
      0x0EDB8832U, 0x79DCB8A4U, 0xE0D5E91EU, 0x97D2D988U, 0x09B64C2BU, 0x7EB17CBDU, 0xE7B82D07U, 0x90BF1D91U,
      0x1DB71064U, 0x6AB020F2U, 0xF3B97148U, 0x84BE41DEU, 0x1ADAD47DU, 0x6DDDE4EBU, 0xF4D4B551U, 0x83D385C7U,
      0x136C9856U, 0x646BA8C0U, 0xFD62F97AU, 0x8A65C9ECU, 0x14015C4FU, 0x63066CD9U, 0xFA0F3D63U, 0x8D080DF5U,
      0x3B6E20C8U, 0x4C69105EU, 0xD56041E4U, 0xA2677172U, 0x3C03E4D1U, 0x4B04D447U, 0xD20D85FDU, 0xA50AB56BU,
      0x35B5A8FAU, 0x42B2986CU, 0xDBBBC9D6U, 0xACBCF940U, 0x32D86CE3U, 0x45DF5C75U, 0xDCD60DCFU, 0xABD13D59U,
      0x26D930ACU, 0x51DE003AU, 0xC8D75180U, 0xBFD06116U, 0x21B4F4B5U, 0x56B3C423U, 0xCFBA9599U, 0xB8BDA50FU,
      0x2802B89EU, 0x5F058808U, 0xC60CD9B2U, 0xB10BE924U, 0x2F6F7C87U, 0x58684C11U, 0xC1611DABU, 0xB6662D3DU,
      0x76DC4190U, 0x01DB7106U, 0x98D220BCU, 0xEFD5102AU, 0x71B18589U, 0x06B6B51FU, 0x9FBFE4A5U, 0xE8B8D433U,
      0x7807C9A2U, 0x0F00F934U, 0x9609A88EU, 0xE10E9818U, 0x7F6A0DBBU, 0x086D3D2DU, 0x91646C97U, 0xE6635C01U,
      0x6B6B51F4U, 0x1C6C6162U, 0x856530D8U, 0xF262004EU, 0x6C0695EDU, 0x1B01A57BU, 0x8208F4C1U, 0xF50FC457U,
      0x65B0D9C6U, 0x12B7E950U, 0x8BBEB8EAU, 0xFCB9887CU, 0x62DD1DDFU, 0x15DA2D49U, 0x8CD37CF3U, 0xFBD44C65U,
      0x4DB26158U, 0x3AB551CEU, 0xA3BC0074U, 0xD4BB30E2U, 0x4ADFA541U, 0x3DD895D7U, 0xA4D1C46DU, 0xD3D6F4FBU,
      0x4369E96AU, 0x346ED9FCU, 0xAD678846U, 0xDA60B8D0U, 0x44042D73U, 0x33031DE5U, 0xAA0A4C5FU, 0xDD0D7CC9U,
      0x5005713CU, 0x270241AAU, 0xBE0B1010U, 0xC90C2086U, 0x5768B525U, 0x206F85B3U, 0xB966D409U, 0xCE61E49FU,
      0x5EDEF90EU, 0x29D9C998U, 0xB0D09822U, 0xC7D7A8B4U, 0x59B33D17U, 0x2EB40D81U, 0xB7BD5C3BU, 0xC0BA6CADU,
      0xEDB88320U, 0x9ABFB3B6U, 0x03B6E20CU, 0x74B1D29AU, 0xEAD54739U, 0x9DD277AFU, 0x04DB2615U, 0x73DC1683U,
      0xE3630B12U, 0x94643B84U, 0x0D6D6A3EU, 0x7A6A5AA8U, 0xE40ECF0BU, 0x9309FF9DU, 0x0A00AE27U, 0x7D079EB1U,
      0xF00F9344U, 0x8708A3D2U, 0x1E01F268U, 0x6906C2FEU, 0xF762575DU, 0x806567CBU, 0x196C3671U, 0x6E6B06E7U,
      0xFED41B76U, 0x89D32BE0U, 0x10DA7A5AU, 0x67DD4ACCU, 0xF9B9DF6FU, 0x8EBEEFF9U, 0x17B7BE43U, 0x60B08ED5U,
      0xD6D6A3E8U, 0xA1D1937EU, 0x38D8C2C4U, 0x4FDFF252U, 0xD1BB67F1U, 0xA6BC5767U, 0x3FB506DDU, 0x48B2364BU,
      0xD80D2BDAU, 0xAF0A1B4CU, 0x36034AF6U, 0x41047A60U, 0xDF60EFC3U, 0xA867DF55U, 0x316E8EEFU, 0x4669BE79U,
      0xCB61B38CU, 0xBC66831AU, 0x256FD2A0U, 0x5268E236U, 0xCC0C7795U, 0xBB0B4703U, 0x220216B9U, 0x5505262FU,
      0xC5BA3BBEU, 0xB2BD0B28U, 0x2BB45A92U, 0x5CB36A04U, 0xC2D7FFA7U, 0xB5D0CF31U, 0x2CD99E8BU, 0x5BDEAE1DU,
      0x9B64C2B0U, 0xEC63F226U, 0x756AA39CU, 0x026D930AU, 0x9C0906A9U, 0xEB0E363FU, 0x72076785U, 0x05005713U,
      0x95BF4A82U, 0xE2B87A14U, 0x7BB12BAEU, 0x0CB61B38U, 0x92D28E9BU, 0xE5D5BE0DU, 0x7CDCEFB7U, 0x0BDBDF21U,
      0x86D3D2D4U, 0xF1D4E242U, 0x68DDB3F8U, 0x1FDA836EU, 0x81BE16CDU, 0xF6B9265BU, 0x6FB077E1U, 0x18B74777U,
      0x88085AE6U, 0xFF0F6A70U, 0x66063BCAU, 0x11010B5CU, 0x8F659EFFU, 0xF862AE69U, 0x616BFFD3U, 0x166CCF45U,
      0xA00AE278U, 0xD70DD2EEU, 0x4E048354U, 0x3903B3C2U, 0xA7672661U, 0xD06016F7U, 0x4969474DU, 0x3E6E77DBU,
      0xAED16A4AU, 0xD9D65ADCU, 0x40DF0B66U, 0x37D83BF0U, 0xA9BCAE53U, 0xDEBB9EC5U, 0x47B2CF7FU, 0x30B5FFE9U,
      0xBDBDF21CU, 0xCABAC28AU, 0x53B39330U, 0x24B4A3A6U, 0xBAD03605U, 0xCDD70693U, 0x54DE5729U, 0x23D967BFU,
      0xB3667A2EU, 0xC4614AB8U, 0x5D681B02U, 0x2A6F2B94U, 0xB40BBE37U, 0xC30C8EA1U, 0x5A05DF1BU, 0x2D02EF8DU,
  };

  uint32_t ulCrc32;

  if (pBuffer == NULL) {
    ulCrc32 = SUSPICIOUS_CRC_VALUE;
  } else {
    const uint8_t *pbBuffer = pBuffer;
    uint32_t ulIdx;

    ulCrc32 = ~ulInitCrc32;

    for (ulIdx = 0U; ulIdx < ulLength; ++ulIdx) {
      ulCrc32 = (ulCrc32 >> 8U) ^ aulCrc32Table[(ulCrc32 ^ pbBuffer[ulIdx]) & 0xFFU];
    }

    ulCrc32 = ~ulCrc32;
  }

  return ulCrc32;
}

#elif REDCONF_CRC_ALGORITHM == CRC_SLICEBY8

/** @brief Compute a CRC32 for the given data buffer.

    For CCITT-32 compliance, the initial CRC must be set to 0.  To CRC multiple
    buffers, call this function with the previously returned CRC value.

    @param ulInitCrc32  Starting CRC value.
    @param pBuffer      Data buffer to calculate the CRC from.
    @param ulLength     Number of bytes of data in the given buffer.

    @return The updated CRC value.
*/
static uint32_t RedCrc32Update(uint32_t ulInitCrc32, const void *pBuffer, uint32_t ulLength) {
  /*  CRC32 XOR table, with slicing-by-8 extensions.

      This first column of the table contains the same XOR values as used in
      the classic byte-at-a-time Sarwate algorithm.  The other seven columns
      are derived from the first, and are used in Intel's slicing-by-eight CRC
      algorithm.

      The layout of this array in memory is novel and deserves explanation.
      In other implementations, including Intel's example, each of the below
      columns is an array.  The first column is a 256-entry array, followed by
      another 256-entry array for the second column, etc.  Testing on both ARM
      and x86 has shown the below mixed arrangement to be about 5-9% faster.
      One possible explanation: With the array-per-table approach, each of the
      eight table lookups is guaranteed to be in a separate 1 KB chunk of
      memory.  With the below array, sometimes multiple table lookups will, by
      coincidence, be close together, making better use of the cache.
  */
  static const uint32_t aulCrc32Table[] = {
      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
      0x77073096U, 0x191B3141U, 0x01C26A37U, 0xB8BC6765U, 0x3D6029B0U, 0xCB5CD3A5U, 0xA6770BB4U, 0xCCAA009EU,
      0xEE0E612CU, 0x32366282U, 0x0384D46EU, 0xAA09C88BU, 0x7AC05360U, 0x4DC8A10BU, 0x979F1129U, 0x4225077DU,
      0x990951BAU, 0x2B2D53C3U, 0x0246BE59U, 0x12B5AFEEU, 0x47A07AD0U, 0x869472AEU, 0x31E81A9DU, 0x8E8F07E3U,
      0x076DC419U, 0x646CC504U, 0x0709A8DCU, 0x8F629757U, 0xF580A6C0U, 0x9B914216U, 0xF44F2413U, 0x844A0EFAU,
      0x706AF48FU, 0x7D77F445U, 0x06CBC2EBU, 0x37DEF032U, 0xC8E08F70U, 0x50CD91B3U, 0x52382FA7U, 0x48E00E64U,
      0xE963A535U, 0x565AA786U, 0x048D7CB2U, 0x256B5FDCU, 0x8F40F5A0U, 0xD659E31DU, 0x63D0353AU, 0xC66F0987U,
      0x9E6495A3U, 0x4F4196C7U, 0x054F1685U, 0x9DD738B9U, 0xB220DC10U, 0x1D0530B8U, 0xC5A73E8EU, 0x0AC50919U,
      0x0EDB8832U, 0xC8D98A08U, 0x0E1351B8U, 0xC5B428EFU, 0x30704BC1U, 0xEC53826DU, 0x33EF4E67U, 0xD3E51BB5U,
      0x79DCB8A4U, 0xD1C2BB49U, 0x0FD13B8FU, 0x7D084F8AU, 0x0D106271U, 0x270F51C8U, 0x959845D3U, 0x1F4F1B2BU,
      0xE0D5E91EU, 0xFAEFE88AU, 0x0D9785D6U, 0x6FBDE064U, 0x4AB018A1U, 0xA19B2366U, 0xA4705F4EU, 0x91C01CC8U,
      0x97D2D988U, 0xE3F4D9CBU, 0x0C55EFE1U, 0xD7018701U, 0x77D03111U, 0x6AC7F0C3U, 0x020754FAU, 0x5D6A1C56U,
      0x09B64C2BU, 0xACB54F0CU, 0x091AF964U, 0x4AD6BFB8U, 0xC5F0ED01U, 0x77C2C07BU, 0xC7A06A74U, 0x57AF154FU,
      0x7EB17CBDU, 0xB5AE7E4DU, 0x08D89353U, 0xF26AD8DDU, 0xF890C4B1U, 0xBC9E13DEU, 0x61D761C0U, 0x9B0515D1U,
      0xE7B82D07U, 0x9E832D8EU, 0x0A9E2D0AU, 0xE0DF7733U, 0xBF30BE61U, 0x3A0A6170U, 0x503F7B5DU, 0x158A1232U,
      0x90BF1D91U, 0x87981CCFU, 0x0B5C473DU, 0x58631056U, 0x825097D1U, 0xF156B2D5U, 0xF64870E9U, 0xD92012ACU,
      0x1DB71064U, 0x4AC21251U, 0x1C26A370U, 0x5019579FU, 0x60E09782U, 0x03D6029BU, 0x67DE9CCEU, 0x7CBB312BU,
      0x6AB020F2U, 0x53D92310U, 0x1DE4C947U, 0xE8A530FAU, 0x5D80BE32U, 0xC88AD13EU, 0xC1A9977AU, 0xB01131B5U,
      0xF3B97148U, 0x78F470D3U, 0x1FA2771EU, 0xFA109F14U, 0x1A20C4E2U, 0x4E1EA390U, 0xF0418DE7U, 0x3E9E3656U,
      0x84BE41DEU, 0x61EF4192U, 0x1E601D29U, 0x42ACF871U, 0x2740ED52U, 0x85427035U, 0x56368653U, 0xF23436C8U,
      0x1ADAD47DU, 0x2EAED755U, 0x1B2F0BACU, 0xDF7BC0C8U, 0x95603142U, 0x9847408DU, 0x9391B8DDU, 0xF8F13FD1U,
      0x6DDDE4EBU, 0x37B5E614U, 0x1AED619BU, 0x67C7A7ADU, 0xA80018F2U, 0x531B9328U, 0x35E6B369U, 0x345B3F4FU,
      0xF4D4B551U, 0x1C98B5D7U, 0x18ABDFC2U, 0x75720843U, 0xEFA06222U, 0xD58FE186U, 0x040EA9F4U, 0xBAD438ACU,
      0x83D385C7U, 0x05838496U, 0x1969B5F5U, 0xCDCE6F26U, 0xD2C04B92U, 0x1ED33223U, 0xA279A240U, 0x767E3832U,
      0x136C9856U, 0x821B9859U, 0x1235F2C8U, 0x95AD7F70U, 0x5090DC43U, 0xEF8580F6U, 0x5431D2A9U, 0xAF5E2A9EU,
      0x646BA8C0U, 0x9B00A918U, 0x13F798FFU, 0x2D111815U, 0x6DF0F5F3U, 0x24D95353U, 0xF246D91DU, 0x63F42A00U,
      0xFD62F97AU, 0xB02DFADBU, 0x11B126A6U, 0x3FA4B7FBU, 0x2A508F23U, 0xA24D21FDU, 0xC3AEC380U, 0xED7B2DE3U,
      0x8A65C9ECU, 0xA936CB9AU, 0x10734C91U, 0x8718D09EU, 0x1730A693U, 0x6911F258U, 0x65D9C834U, 0x21D12D7DU,
      0x14015C4FU, 0xE6775D5DU, 0x153C5A14U, 0x1ACFE827U, 0xA5107A83U, 0x7414C2E0U, 0xA07EF6BAU, 0x2B142464U,
      0x63066CD9U, 0xFF6C6C1CU, 0x14FE3023U, 0xA2738F42U, 0x98705333U, 0xBF481145U, 0x0609FD0EU, 0xE7BE24FAU,
      0xFA0F3D63U, 0xD4413FDFU, 0x16B88E7AU, 0xB0C620ACU, 0xDFD029E3U, 0x39DC63EBU, 0x37E1E793U, 0x69312319U,
      0x8D080DF5U, 0xCD5A0E9EU, 0x177AE44DU, 0x087A47C9U, 0xE2B00053U, 0xF280B04EU, 0x9196EC27U, 0xA59B2387U,
      0x3B6E20C8U, 0x958424A2U, 0x384D46E0U, 0xA032AF3EU, 0xC1C12F04U, 0x07AC0536U, 0xCFBD399CU, 0xF9766256U,
      0x4C69105EU, 0x8C9F15E3U, 0x398F2CD7U, 0x188EC85BU, 0xFCA106B4U, 0xCCF0D693U, 0x69CA3228U, 0x35DC62C8U,
      0xD56041E4U, 0xA7B24620U, 0x3BC9928EU, 0x0A3B67B5U, 0xBB017C64U, 0x4A64A43DU, 0x582228B5U, 0xBB53652BU,
      0xA2677172U, 0xBEA97761U, 0x3A0BF8B9U, 0xB28700D0U, 0x866155D4U, 0x81387798U, 0xFE552301U, 0x77F965B5U,
      0x3C03E4D1U, 0xF1E8E1A6U, 0x3F44EE3CU, 0x2F503869U, 0x344189C4U, 0x9C3D4720U, 0x3BF21D8FU, 0x7D3C6CACU,
      0x4B04D447U, 0xE8F3D0E7U, 0x3E86840BU, 0x97EC5F0CU, 0x0921A074U, 0x57619485U, 0x9D85163BU, 0xB1966C32U,
      0xD20D85FDU, 0xC3DE8324U, 0x3CC03A52U, 0x8559F0E2U, 0x4E81DAA4U, 0xD1F5E62BU, 0xAC6D0CA6U, 0x3F196BD1U,
      0xA50AB56BU, 0xDAC5B265U, 0x3D025065U, 0x3DE59787U, 0x73E1F314U, 0x1AA9358EU, 0x0A1A0712U, 0xF3B36B4FU,
      0x35B5A8FAU, 0x5D5DAEAAU, 0x365E1758U, 0x658687D1U, 0xF1B164C5U, 0xEBFF875BU, 0xFC5277FBU, 0x2A9379E3U,
      0x42B2986CU, 0x44469FEBU, 0x379C7D6FU, 0xDD3AE0B4U, 0xCCD14D75U, 0x20A354FEU, 0x5A257C4FU, 0xE639797DU,
      0xDBBBC9D6U, 0x6F6BCC28U, 0x35DAC336U, 0xCF8F4F5AU, 0x8B7137A5U, 0xA6372650U, 0x6BCD66D2U, 0x68B67E9EU,
      0xACBCF940U, 0x7670FD69U, 0x3418A901U, 0x7733283FU, 0xB6111E15U, 0x6D6BF5F5U, 0xCDBA6D66U, 0xA41C7E00U,
      0x32D86CE3U, 0x39316BAEU, 0x3157BF84U, 0xEAE41086U, 0x0431C205U, 0x706EC54DU, 0x081D53E8U, 0xAED97719U,
      0x45DF5C75U, 0x202A5AEFU, 0x3095D5B3U, 0x525877E3U, 0x3951EBB5U, 0xBB3216E8U, 0xAE6A585CU, 0x62737787U,
      0xDCD60DCFU, 0x0B07092CU, 0x32D36BEAU, 0x40EDD80DU, 0x7EF19165U, 0x3DA66446U, 0x9F8242C1U, 0xECFC7064U,
      0xABD13D59U, 0x121C386DU, 0x331101DDU, 0xF851BF68U, 0x4391B8D5U, 0xF6FAB7E3U, 0x39F54975U, 0x205670FAU,
      0x26D930ACU, 0xDF4636F3U, 0x246BE590U, 0xF02BF8A1U, 0xA121B886U, 0x047A07ADU, 0xA863A552U, 0x85CD537DU,
      0x51DE003AU, 0xC65D07B2U, 0x25A98FA7U, 0x48979FC4U, 0x9C419136U, 0xCF26D408U, 0x0E14AEE6U, 0x496753E3U,
      0xC8D75180U, 0xED705471U, 0x27EF31FEU, 0x5A22302AU, 0xDBE1EBE6U, 0x49B2A6A6U, 0x3FFCB47BU, 0xC7E85400U,
      0xBFD06116U, 0xF46B6530U, 0x262D5BC9U, 0xE29E574FU, 0xE681C256U, 0x82EE7503U, 0x998BBFCFU, 0x0B42549EU,
      0x21B4F4B5U, 0xBB2AF3F7U, 0x23624D4CU, 0x7F496FF6U, 0x54A11E46U, 0x9FEB45BBU, 0x5C2C8141U, 0x01875D87U,
      0x56B3C423U, 0xA231C2B6U, 0x22A0277BU, 0xC7F50893U, 0x69C137F6U, 0x54B7961EU, 0xFA5B8AF5U, 0xCD2D5D19U,
      0xCFBA9599U, 0x891C9175U, 0x20E69922U, 0xD540A77DU, 0x2E614D26U, 0xD223E4B0U, 0xCBB39068U, 0x43A25AFAU,
      0xB8BDA50FU, 0x9007A034U, 0x2124F315U, 0x6DFCC018U, 0x13016496U, 0x197F3715U, 0x6DC49BDCU, 0x8F085A64U,
      0x2802B89EU, 0x179FBCFBU, 0x2A78B428U, 0x359FD04EU, 0x9151F347U, 0xE82985C0U, 0x9B8CEB35U, 0x562848C8U,
      0x5F058808U, 0x0E848DBAU, 0x2BBADE1FU, 0x8D23B72BU, 0xAC31DAF7U, 0x23755665U, 0x3DFBE081U, 0x9A824856U,
      0xC60CD9B2U, 0x25A9DE79U, 0x29FC6046U, 0x9F9618C5U, 0xEB91A027U, 0xA5E124CBU, 0x0C13FA1CU, 0x140D4FB5U,
      0xB10BE924U, 0x3CB2EF38U, 0x283E0A71U, 0x272A7FA0U, 0xD6F18997U, 0x6EBDF76EU, 0xAA64F1A8U, 0xD8A74F2BU,
      0x2F6F7C87U, 0x73F379FFU, 0x2D711CF4U, 0xBAFD4719U, 0x64D15587U, 0x73B8C7D6U, 0x6FC3CF26U, 0xD2624632U,
      0x58684C11U, 0x6AE848BEU, 0x2CB376C3U, 0x0241207CU, 0x59B17C37U, 0xB8E41473U, 0xC9B4C492U, 0x1EC846ACU,
      0xC1611DABU, 0x41C51B7DU, 0x2EF5C89AU, 0x10F48F92U, 0x1E1106E7U, 0x3E7066DDU, 0xF85CDE0FU, 0x9047414FU,
      0xB6662D3DU, 0x58DE2A3CU, 0x2F37A2ADU, 0xA848E8F7U, 0x23712F57U, 0xF52CB578U, 0x5E2BD5BBU, 0x5CED41D1U,
      0x76DC4190U, 0xF0794F05U, 0x709A8DC0U, 0x9B14583DU, 0x58F35849U, 0x0F580A6CU, 0x440B7579U, 0x299DC2EDU,
      0x01DB7106U, 0xE9627E44U, 0x7158E7F7U, 0x23A83F58U, 0x659371F9U, 0xC404D9C9U, 0xE27C7ECDU, 0xE537C273U,
      0x98D220BCU, 0xC24F2D87U, 0x731E59AEU, 0x311D90B6U, 0x22330B29U, 0x4290AB67U, 0xD3946450U, 0x6BB8C590U,
      0xEFD5102AU, 0xDB541CC6U, 0x72DC3399U, 0x89A1F7D3U, 0x1F532299U, 0x89CC78C2U, 0x75E36FE4U, 0xA712C50EU,
      0x71B18589U, 0x94158A01U, 0x7793251CU, 0x1476CF6AU, 0xAD73FE89U, 0x94C9487AU, 0xB044516AU, 0xADD7CC17U,
      0x06B6B51FU, 0x8D0EBB40U, 0x76514F2BU, 0xACCAA80FU, 0x9013D739U, 0x5F959BDFU, 0x16335ADEU, 0x617DCC89U,
      0x9FBFE4A5U, 0xA623E883U, 0x7417F172U, 0xBE7F07E1U, 0xD7B3ADE9U, 0xD901E971U, 0x27DB4043U, 0xEFF2CB6AU,
      0xE8B8D433U, 0xBF38D9C2U, 0x75D59B45U, 0x06C36084U, 0xEAD38459U, 0x125D3AD4U, 0x81AC4BF7U, 0x2358CBF4U,
      0x7807C9A2U, 0x38A0C50DU, 0x7E89DC78U, 0x5EA070D2U, 0x68831388U, 0xE30B8801U, 0x77E43B1EU, 0xFA78D958U,
      0x0F00F934U, 0x21BBF44CU, 0x7F4BB64FU, 0xE61C17B7U, 0x55E33A38U, 0x28575BA4U, 0xD19330AAU, 0x36D2D9C6U,
      0x9609A88EU, 0x0A96A78FU, 0x7D0D0816U, 0xF4A9B859U, 0x124340E8U, 0xAEC3290AU, 0xE07B2A37U, 0xB85DDE25U,
      0xE10E9818U, 0x138D96CEU, 0x7CCF6221U, 0x4C15DF3CU, 0x2F236958U, 0x659FFAAFU, 0x460C2183U, 0x74F7DEBBU,
      0x7F6A0DBBU, 0x5CCC0009U, 0x798074A4U, 0xD1C2E785U, 0x9D03B548U, 0x789ACA17U, 0x83AB1F0DU, 0x7E32D7A2U,
      0x086D3D2DU, 0x45D73148U, 0x78421E93U, 0x697E80E0U, 0xA0639CF8U, 0xB3C619B2U, 0x25DC14B9U, 0xB298D73CU,
      0x91646C97U, 0x6EFA628BU, 0x7A04A0CAU, 0x7BCB2F0EU, 0xE7C3E628U, 0x35526B1CU, 0x14340E24U, 0x3C17D0DFU,
      0xE6635C01U, 0x77E153CAU, 0x7BC6CAFDU, 0xC377486BU, 0xDAA3CF98U, 0xFE0EB8B9U, 0xB2430590U, 0xF0BDD041U,
      0x6B6B51F4U, 0xBABB5D54U, 0x6CBC2EB0U, 0xCB0D0FA2U, 0x3813CFCBU, 0x0C8E08F7U, 0x23D5E9B7U, 0x5526F3C6U,
      0x1C6C6162U, 0xA3A06C15U, 0x6D7E4487U, 0x73B168C7U, 0x0573E67BU, 0xC7D2DB52U, 0x85A2E203U, 0x998CF358U,
      0x856530D8U, 0x888D3FD6U, 0x6F38FADEU, 0x6104C729U, 0x42D39CABU, 0x4146A9FCU, 0xB44AF89EU, 0x1703F4BBU,
      0xF262004EU, 0x91960E97U, 0x6EFA90E9U, 0xD9B8A04CU, 0x7FB3B51BU, 0x8A1A7A59U, 0x123DF32AU, 0xDBA9F425U,
      0x6C0695EDU, 0xDED79850U, 0x6BB5866CU, 0x446F98F5U, 0xCD93690BU, 0x971F4AE1U, 0xD79ACDA4U, 0xD16CFD3CU,
      0x1B01A57BU, 0xC7CCA911U, 0x6A77EC5BU, 0xFCD3FF90U, 0xF0F340BBU, 0x5C439944U, 0x71EDC610U, 0x1DC6FDA2U,
      0x8208F4C1U, 0xECE1FAD2U, 0x68315202U, 0xEE66507EU, 0xB7533A6BU, 0xDAD7EBEAU, 0x4005DC8DU, 0x9349FA41U,
      0xF50FC457U, 0xF5FACB93U, 0x69F33835U, 0x56DA371BU, 0x8A3313DBU, 0x118B384FU, 0xE672D739U, 0x5FE3FADFU,
      0x65B0D9C6U, 0x7262D75CU, 0x62AF7F08U, 0x0EB9274DU, 0x0863840AU, 0xE0DD8A9AU, 0x103AA7D0U, 0x86C3E873U,
      0x12B7E950U, 0x6B79E61DU, 0x636D153FU, 0xB6054028U, 0x3503ADBAU, 0x2B81593FU, 0xB64DAC64U, 0x4A69E8EDU,
      0x8BBEB8EAU, 0x4054B5DEU, 0x612BAB66U, 0xA4B0EFC6U, 0x72A3D76AU, 0xAD152B91U, 0x87A5B6F9U, 0xC4E6EF0EU,
      0xFCB9887CU, 0x594F849FU, 0x60E9C151U, 0x1C0C88A3U, 0x4FC3FEDAU, 0x6649F834U, 0x21D2BD4DU, 0x084CEF90U,
      0x62DD1DDFU, 0x160E1258U, 0x65A6D7D4U, 0x81DBB01AU, 0xFDE322CAU, 0x7B4CC88CU, 0xE47583C3U, 0x0289E689U,
      0x15DA2D49U, 0x0F152319U, 0x6464BDE3U, 0x3967D77FU, 0xC0830B7AU, 0xB0101B29U, 0x42028877U, 0xCE23E617U,
      0x8CD37CF3U, 0x243870DAU, 0x662203BAU, 0x2BD27891U, 0x872371AAU, 0x36846987U, 0x73EA92EAU, 0x40ACE1F4U,
      0xFBD44C65U, 0x3D23419BU, 0x67E0698DU, 0x936E1FF4U, 0xBA43581AU, 0xFDD8BA22U, 0xD59D995EU, 0x8C06E16AU,
      0x4DB26158U, 0x65FD6BA7U, 0x48D7CB20U, 0x3B26F703U, 0x9932774DU, 0x08F40F5AU, 0x8BB64CE5U, 0xD0EBA0BBU,
      0x3AB551CEU, 0x7CE65AE6U, 0x4915A117U, 0x839A9066U, 0xA4525EFDU, 0xC3A8DCFFU, 0x2DC14751U, 0x1C41A025U,
      0xA3BC0074U, 0x57CB0925U, 0x4B531F4EU, 0x912F3F88U, 0xE3F2242DU, 0x453CAE51U, 0x1C295DCCU, 0x92CEA7C6U,
      0xD4BB30E2U, 0x4ED03864U, 0x4A917579U, 0x299358EDU, 0xDE920D9DU, 0x8E607DF4U, 0xBA5E5678U, 0x5E64A758U,
      0x4ADFA541U, 0x0191AEA3U, 0x4FDE63FCU, 0xB4446054U, 0x6CB2D18DU, 0x93654D4CU, 0x7FF968F6U, 0x54A1AE41U,
      0x3DD895D7U, 0x188A9FE2U, 0x4E1C09CBU, 0x0CF80731U, 0x51D2F83DU, 0x58399EE9U, 0xD98E6342U, 0x980BAEDFU,
      0xA4D1C46DU, 0x33A7CC21U, 0x4C5AB792U, 0x1E4DA8DFU, 0x167282EDU, 0xDEADEC47U, 0xE86679DFU, 0x1684A93CU,
      0xD3D6F4FBU, 0x2ABCFD60U, 0x4D98DDA5U, 0xA6F1CFBAU, 0x2B12AB5DU, 0x15F13FE2U, 0x4E11726BU, 0xDA2EA9A2U,
      0x4369E96AU, 0xAD24E1AFU, 0x46C49A98U, 0xFE92DFECU, 0xA9423C8CU, 0xE4A78D37U, 0xB8590282U, 0x030EBB0EU,
      0x346ED9FCU, 0xB43FD0EEU, 0x4706F0AFU, 0x462EB889U, 0x9422153CU, 0x2FFB5E92U, 0x1E2E0936U, 0xCFA4BB90U,
      0xAD678846U, 0x9F12832DU, 0x45404EF6U, 0x549B1767U, 0xD3826FECU, 0xA96F2C3CU, 0x2FC613ABU, 0x412BBC73U,
      0xDA60B8D0U, 0x8609B26CU, 0x448224C1U, 0xEC277002U, 0xEEE2465CU, 0x6233FF99U, 0x89B1181FU, 0x8D81BCEDU,
      0x44042D73U, 0xC94824ABU, 0x41CD3244U, 0x71F048BBU, 0x5CC29A4CU, 0x7F36CF21U, 0x4C162691U, 0x8744B5F4U,
      0x33031DE5U, 0xD05315EAU, 0x400F5873U, 0xC94C2FDEU, 0x61A2B3FCU, 0xB46A1C84U, 0xEA612D25U, 0x4BEEB56AU,
      0xAA0A4C5FU, 0xFB7E4629U, 0x4249E62AU, 0xDBF98030U, 0x2602C92CU, 0x32FE6E2AU, 0xDB8937B8U, 0xC561B289U,
      0xDD0D7CC9U, 0xE2657768U, 0x438B8C1DU, 0x6345E755U, 0x1B62E09CU, 0xF9A2BD8FU, 0x7DFE3C0CU, 0x09CBB217U,
      0x5005713CU, 0x2F3F79F6U, 0x54F16850U, 0x6B3FA09CU, 0xF9D2E0CFU, 0x0B220DC1U, 0xEC68D02BU, 0xAC509190U,
      0x270241AAU, 0x362448B7U, 0x55330267U, 0xD383C7F9U, 0xC4B2C97FU, 0xC07EDE64U, 0x4A1FDB9FU, 0x60FA910EU,
      0xBE0B1010U, 0x1D091B74U, 0x5775BC3EU, 0xC1366817U, 0x8312B3AFU, 0x46EAACCAU, 0x7BF7C102U, 0xEE7596EDU,
      0xC90C2086U, 0x04122A35U, 0x56B7D609U, 0x798A0F72U, 0xBE729A1FU, 0x8DB67F6FU, 0xDD80CAB6U, 0x22DF9673U,
      0x5768B525U, 0x4B53BCF2U, 0x53F8C08CU, 0xE45D37CBU, 0x0C52460FU, 0x90B34FD7U, 0x1827F438U, 0x281A9F6AU,
      0x206F85B3U, 0x52488DB3U, 0x523AAABBU, 0x5CE150AEU, 0x31326FBFU, 0x5BEF9C72U, 0xBE50FF8CU, 0xE4B09FF4U,
      0xB966D409U, 0x7965DE70U, 0x507C14E2U, 0x4E54FF40U, 0x7692156FU, 0xDD7BEEDCU, 0x8FB8E511U, 0x6A3F9817U,
      0xCE61E49FU, 0x607EEF31U, 0x51BE7ED5U, 0xF6E89825U, 0x4BF23CDFU, 0x16273D79U, 0x29CFEEA5U, 0xA6959889U,
      0x5EDEF90EU, 0xE7E6F3FEU, 0x5AE239E8U, 0xAE8B8873U, 0xC9A2AB0EU, 0xE7718FACU, 0xDF879E4CU, 0x7FB58A25U,
      0x29D9C998U, 0xFEFDC2BFU, 0x5B2053DFU, 0x1637EF16U, 0xF4C282BEU, 0x2C2D5C09U, 0x79F095F8U, 0xB31F8ABBU,
      0xB0D09822U, 0xD5D0917CU, 0x5966ED86U, 0x048240F8U, 0xB362F86EU, 0xAAB92EA7U, 0x48188F65U, 0x3D908D58U,
      0xC7D7A8B4U, 0xCCCBA03DU, 0x58A487B1U, 0xBC3E279DU, 0x8E02D1DEU, 0x61E5FD02U, 0xEE6F84D1U, 0xF13A8DC6U,
      0x59B33D17U, 0x838A36FAU, 0x5DEB9134U, 0x21E91F24U, 0x3C220DCEU, 0x7CE0CDBAU, 0x2BC8BA5FU, 0xFBFF84DFU,
      0x2EB40D81U, 0x9A9107BBU, 0x5C29FB03U, 0x99557841U, 0x0142247EU, 0xB7BC1E1FU, 0x8DBFB1EBU, 0x37558441U,
      0xB7BD5C3BU, 0xB1BC5478U, 0x5E6F455AU, 0x8BE0D7AFU, 0x46E25EAEU, 0x31286CB1U, 0xBC57AB76U, 0xB9DA83A2U,
      0xC0BA6CADU, 0xA8A76539U, 0x5FAD2F6DU, 0x335CB0CAU, 0x7B82771EU, 0xFA74BF14U, 0x1A20A0C2U, 0x7570833CU,
      0xEDB88320U, 0x3B83984BU, 0xE1351B80U, 0xED59B63BU, 0xB1E6B092U, 0x1EB014D8U, 0x8816EAF2U, 0x533B85DAU,
      0x9ABFB3B6U, 0x2298A90AU, 0xE0F771B7U, 0x55E5D15EU, 0x8C869922U, 0xD5ECC77DU, 0x2E61E146U, 0x9F918544U,
      0x03B6E20CU, 0x09B5FAC9U, 0xE2B1CFEEU, 0x47507EB0U, 0xCB26E3F2U, 0x5378B5D3U, 0x1F89FBDBU, 0x111E82A7U,
      0x74B1D29AU, 0x10AECB88U, 0xE373A5D9U, 0xFFEC19D5U, 0xF646CA42U, 0x98246676U, 0xB9FEF06FU, 0xDDB48239U,
      0xEAD54739U, 0x5FEF5D4FU, 0xE63CB35CU, 0x623B216CU, 0x44661652U, 0x852156CEU, 0x7C59CEE1U, 0xD7718B20U,
      0x9DD277AFU, 0x46F46C0EU, 0xE7FED96BU, 0xDA874609U, 0x79063FE2U, 0x4E7D856BU, 0xDA2EC555U, 0x1BDB8BBEU,
      0x04DB2615U, 0x6DD93FCDU, 0xE5B86732U, 0xC832E9E7U, 0x3EA64532U, 0xC8E9F7C5U, 0xEBC6DFC8U, 0x95548C5DU,
      0x73DC1683U, 0x74C20E8CU, 0xE47A0D05U, 0x708E8E82U, 0x03C66C82U, 0x03B52460U, 0x4DB1D47CU, 0x59FE8CC3U,
      0xE3630B12U, 0xF35A1243U, 0xEF264A38U, 0x28ED9ED4U, 0x8196FB53U, 0xF2E396B5U, 0xBBF9A495U, 0x80DE9E6FU,
      0x94643B84U, 0xEA412302U, 0xEEE4200FU, 0x9051F9B1U, 0xBCF6D2E3U, 0x39BF4510U, 0x1D8EAF21U, 0x4C749EF1U,
      0x0D6D6A3EU, 0xC16C70C1U, 0xECA29E56U, 0x82E4565FU, 0xFB56A833U, 0xBF2B37BEU, 0x2C66B5BCU, 0xC2FB9912U,
      0x7A6A5AA8U, 0xD8774180U, 0xED60F461U, 0x3A58313AU, 0xC6368183U, 0x7477E41BU, 0x8A11BE08U, 0x0E51998CU,
      0xE40ECF0BU, 0x9736D747U, 0xE82FE2E4U, 0xA78F0983U, 0x74165D93U, 0x6972D4A3U, 0x4FB68086U, 0x04949095U,
      0x9309FF9DU, 0x8E2DE606U, 0xE9ED88D3U, 0x1F336EE6U, 0x49767423U, 0xA22E0706U, 0xE9C18B32U, 0xC83E900BU,
      0x0A00AE27U, 0xA500B5C5U, 0xEBAB368AU, 0x0D86C108U, 0x0ED60EF3U, 0x24BA75A8U, 0xD82991AFU, 0x46B197E8U,
      0x7D079EB1U, 0xBC1B8484U, 0xEA695CBDU, 0xB53AA66DU, 0x33B62743U, 0xEFE6A60DU, 0x7E5E9A1BU, 0x8A1B9776U,
      0xF00F9344U, 0x71418A1AU, 0xFD13B8F0U, 0xBD40E1A4U, 0xD1062710U, 0x1D661643U, 0xEFC8763CU, 0x2F80B4F1U,
      0x8708A3D2U, 0x685ABB5BU, 0xFCD1D2C7U, 0x05FC86C1U, 0xEC660EA0U, 0xD63AC5E6U, 0x49BF7D88U, 0xE32AB46FU,
      0x1E01F268U, 0x4377E898U, 0xFE976C9EU, 0x1749292FU, 0xABC67470U, 0x50AEB748U, 0x78576715U, 0x6DA5B38CU,
      0x6906C2FEU, 0x5A6CD9D9U, 0xFF5506A9U, 0xAFF54E4AU, 0x96A65DC0U, 0x9BF264EDU, 0xDE206CA1U, 0xA10FB312U,
      0xF762575DU, 0x152D4F1EU, 0xFA1A102CU, 0x322276F3U, 0x248681D0U, 0x86F75455U, 0x1B87522FU, 0xABCABA0BU,
      0x806567CBU, 0x0C367E5FU, 0xFBD87A1BU, 0x8A9E1196U, 0x19E6A860U, 0x4DAB87F0U, 0xBDF0599BU, 0x6760BA95U,
      0x196C3671U, 0x271B2D9CU, 0xF99EC442U, 0x982BBE78U, 0x5E46D2B0U, 0xCB3FF55EU, 0x8C184306U, 0xE9EFBD76U,
      0x6E6B06E7U, 0x3E001CDDU, 0xF85CAE75U, 0x2097D91DU, 0x6326FB00U, 0x006326FBU, 0x2A6F48B2U, 0x2545BDE8U,
      0xFED41B76U, 0xB9980012U, 0xF300E948U, 0x78F4C94BU, 0xE1766CD1U, 0xF135942EU, 0xDC27385BU, 0xFC65AF44U,
      0x89D32BE0U, 0xA0833153U, 0xF2C2837FU, 0xC048AE2EU, 0xDC164561U, 0x3A69478BU, 0x7A5033EFU, 0x30CFAFDAU,
      0x10DA7A5AU, 0x8BAE6290U, 0xF0843D26U, 0xD2FD01C0U, 0x9BB63FB1U, 0xBCFD3525U, 0x4BB82972U, 0xBE40A839U,
      0x67DD4ACCU, 0x92B553D1U, 0xF1465711U, 0x6A4166A5U, 0xA6D61601U, 0x77A1E680U, 0xEDCF22C6U, 0x72EAA8A7U,
      0xF9B9DF6FU, 0xDDF4C516U, 0xF4094194U, 0xF7965E1CU, 0x14F6CA11U, 0x6AA4D638U, 0x28681C48U, 0x782FA1BEU,
      0x8EBEEFF9U, 0xC4EFF457U, 0xF5CB2BA3U, 0x4F2A3979U, 0x2996E3A1U, 0xA1F8059DU, 0x8E1F17FCU, 0xB485A120U,
      0x17B7BE43U, 0xEFC2A794U, 0xF78D95FAU, 0x5D9F9697U, 0x6E369971U, 0x276C7733U, 0xBFF70D61U, 0x3A0AA6C3U,
      0x60B08ED5U, 0xF6D996D5U, 0xF64FFFCDU, 0xE523F1F2U, 0x5356B0C1U, 0xEC30A496U, 0x198006D5U, 0xF6A0A65DU,
      0xD6D6A3E8U, 0xAE07BCE9U, 0xD9785D60U, 0x4D6B1905U, 0x70279F96U, 0x191C11EEU, 0x47ABD36EU, 0xAA4DE78CU,
      0xA1D1937EU, 0xB71C8DA8U, 0xD8BA3757U, 0xF5D77E60U, 0x4D47B626U, 0xD240C24BU, 0xE1DCD8DAU, 0x66E7E712U,
      0x38D8C2C4U, 0x9C31DE6BU, 0xDAFC890EU, 0xE762D18EU, 0x0AE7CCF6U, 0x54D4B0E5U, 0xD034C247U, 0xE868E0F1U,
      0x4FDFF252U, 0x852AEF2AU, 0xDB3EE339U, 0x5FDEB6EBU, 0x3787E546U, 0x9F886340U, 0x7643C9F3U, 0x24C2E06FU,
      0xD1BB67F1U, 0xCA6B79EDU, 0xDE71F5BCU, 0xC2098E52U, 0x85A73956U, 0x828D53F8U, 0xB3E4F77DU, 0x2E07E976U,
      0xA6BC5767U, 0xD37048ACU, 0xDFB39F8BU, 0x7AB5E937U, 0xB8C710E6U, 0x49D1805DU, 0x1593FCC9U, 0xE2ADE9E8U,
      0x3FB506DDU, 0xF85D1B6FU, 0xDDF521D2U, 0x680046D9U, 0xFF676A36U, 0xCF45F2F3U, 0x247BE654U, 0x6C22EE0BU,
      0x48B2364BU, 0xE1462A2EU, 0xDC374BE5U, 0xD0BC21BCU, 0xC2074386U, 0x04192156U, 0x820CEDE0U, 0xA088EE95U,
      0xD80D2BDAU, 0x66DE36E1U, 0xD76B0CD8U, 0x88DF31EAU, 0x4057D457U, 0xF54F9383U, 0x74449D09U, 0x79A8FC39U,
      0xAF0A1B4CU, 0x7FC507A0U, 0xD6A966EFU, 0x3063568FU, 0x7D37FDE7U, 0x3E134026U, 0xD23396BDU, 0xB502FCA7U,
      0x36034AF6U, 0x54E85463U, 0xD4EFD8B6U, 0x22D6F961U, 0x3A978737U, 0xB8873288U, 0xE3DB8C20U, 0x3B8DFB44U,
      0x41047A60U, 0x4DF36522U, 0xD52DB281U, 0x9A6A9E04U, 0x07F7AE87U, 0x73DBE12DU, 0x45AC8794U, 0xF727FBDAU,
      0xDF60EFC3U, 0x02B2F3E5U, 0xD062A404U, 0x07BDA6BDU, 0xB5D77297U, 0x6EDED195U, 0x800BB91AU, 0xFDE2F2C3U,
      0xA867DF55U, 0x1BA9C2A4U, 0xD1A0CE33U, 0xBF01C1D8U, 0x88B75B27U, 0xA5820230U, 0x267CB2AEU, 0x3148F25DU,
      0x316E8EEFU, 0x30849167U, 0xD3E6706AU, 0xADB46E36U, 0xCF1721F7U, 0x2316709EU, 0x1794A833U, 0xBFC7F5BEU,
      0x4669BE79U, 0x299FA026U, 0xD2241A5DU, 0x15080953U, 0xF2770847U, 0xE84AA33BU, 0xB1E3A387U, 0x736DF520U,
      0xCB61B38CU, 0xE4C5AEB8U, 0xC55EFE10U, 0x1D724E9AU, 0x10C70814U, 0x1ACA1375U, 0x20754FA0U, 0xD6F6D6A7U,
      0xBC66831AU, 0xFDDE9FF9U, 0xC49C9427U, 0xA5CE29FFU, 0x2DA721A4U, 0xD196C0D0U, 0x86024414U, 0x1A5CD639U,
      0x256FD2A0U, 0xD6F3CC3AU, 0xC6DA2A7EU, 0xB77B8611U, 0x6A075B74U, 0x5702B27EU, 0xB7EA5E89U, 0x94D3D1DAU,
      0x5268E236U, 0xCFE8FD7BU, 0xC7184049U, 0x0FC7E174U, 0x576772C4U, 0x9C5E61DBU, 0x119D553DU, 0x5879D144U,
      0xCC0C7795U, 0x80A96BBCU, 0xC25756CCU, 0x9210D9CDU, 0xE547AED4U, 0x815B5163U, 0xD43A6BB3U, 0x52BCD85DU,
      0xBB0B4703U, 0x99B25AFDU, 0xC3953CFBU, 0x2AACBEA8U, 0xD8278764U, 0x4A0782C6U, 0x724D6007U, 0x9E16D8C3U,
      0x220216B9U, 0xB29F093EU, 0xC1D382A2U, 0x38191146U, 0x9F87FDB4U, 0xCC93F068U, 0x43A57A9AU, 0x1099DF20U,
      0x5505262FU, 0xAB84387FU, 0xC011E895U, 0x80A57623U, 0xA2E7D404U, 0x07CF23CDU, 0xE5D2712EU, 0xDC33DFBEU,
      0xC5BA3BBEU, 0x2C1C24B0U, 0xCB4DAFA8U, 0xD8C66675U, 0x20B743D5U, 0xF6999118U, 0x139A01C7U, 0x0513CD12U,
      0xB2BD0B28U, 0x350715F1U, 0xCA8FC59FU, 0x607A0110U, 0x1DD76A65U, 0x3DC542BDU, 0xB5ED0A73U, 0xC9B9CD8CU,
      0x2BB45A92U, 0x1E2A4632U, 0xC8C97BC6U, 0x72CFAEFEU, 0x5A7710B5U, 0xBB513013U, 0x840510EEU, 0x4736CA6FU,
      0x5CB36A04U, 0x07317773U, 0xC90B11F1U, 0xCA73C99BU, 0x67173905U, 0x700DE3B6U, 0x22721B5AU, 0x8B9CCAF1U,
      0xC2D7FFA7U, 0x4870E1B4U, 0xCC440774U, 0x57A4F122U, 0xD537E515U, 0x6D08D30EU, 0xE7D525D4U, 0x8159C3E8U,
      0xB5D0CF31U, 0x516BD0F5U, 0xCD866D43U, 0xEF189647U, 0xE857CCA5U, 0xA65400ABU, 0x41A22E60U, 0x4DF3C376U,
      0x2CD99E8BU, 0x7A468336U, 0xCFC0D31AU, 0xFDAD39A9U, 0xAFF7B675U, 0x20C07205U, 0x704A34FDU, 0xC37CC495U,
      0x5BDEAE1DU, 0x635DB277U, 0xCE02B92DU, 0x45115ECCU, 0x92979FC5U, 0xEB9CA1A0U, 0xD63D3F49U, 0x0FD6C40BU,
      0x9B64C2B0U, 0xCBFAD74EU, 0x91AF9640U, 0x764DEE06U, 0xE915E8DBU, 0x11E81EB4U, 0xCC1D9F8BU, 0x7AA64737U,
      0xEC63F226U, 0xD2E1E60FU, 0x906DFC77U, 0xCEF18963U, 0xD475C16BU, 0xDAB4CD11U, 0x6A6A943FU, 0xB60C47A9U,
      0x756AA39CU, 0xF9CCB5CCU, 0x922B422EU, 0xDC44268DU, 0x93D5BBBBU, 0x5C20BFBFU, 0x5B828EA2U, 0x3883404AU,
      0x026D930AU, 0xE0D7848DU, 0x93E92819U, 0x64F841E8U, 0xAEB5920BU, 0x977C6C1AU, 0xFDF58516U, 0xF42940D4U,
      0x9C0906A9U, 0xAF96124AU, 0x96A63E9CU, 0xF92F7951U, 0x1C954E1BU, 0x8A795CA2U, 0x3852BB98U, 0xFEEC49CDU,
      0xEB0E363FU, 0xB68D230BU, 0x976454ABU, 0x41931E34U, 0x21F567ABU, 0x41258F07U, 0x9E25B02CU, 0x32464953U,
      0x72076785U, 0x9DA070C8U, 0x9522EAF2U, 0x5326B1DAU, 0x66551D7BU, 0xC7B1FDA9U, 0xAFCDAAB1U, 0xBCC94EB0U,
      0x05005713U, 0x84BB4189U, 0x94E080C5U, 0xEB9AD6BFU, 0x5B3534CBU, 0x0CED2E0CU, 0x09BAA105U, 0x70634E2EU,
      0x95BF4A82U, 0x03235D46U, 0x9FBCC7F8U, 0xB3F9C6E9U, 0xD965A31AU, 0xFDBB9CD9U, 0xFFF2D1ECU, 0xA9435C82U,
      0xE2B87A14U, 0x1A386C07U, 0x9E7EADCFU, 0x0B45A18CU, 0xE4058AAAU, 0x36E74F7CU, 0x5985DA58U, 0x65E95C1CU,
      0x7BB12BAEU, 0x31153FC4U, 0x9C381396U, 0x19F00E62U, 0xA3A5F07AU, 0xB0733DD2U, 0x686DC0C5U, 0xEB665BFFU,
      0x0CB61B38U, 0x280E0E85U, 0x9DFA79A1U, 0xA14C6907U, 0x9EC5D9CAU, 0x7B2FEE77U, 0xCE1ACB71U, 0x27CC5B61U,
      0x92D28E9BU, 0x674F9842U, 0x98B56F24U, 0x3C9B51BEU, 0x2CE505DAU, 0x662ADECFU, 0x0BBDF5FFU, 0x2D095278U,
      0xE5D5BE0DU, 0x7E54A903U, 0x99770513U, 0x842736DBU, 0x11852C6AU, 0xAD760D6AU, 0xADCAFE4BU, 0xE1A352E6U,
      0x7CDCEFB7U, 0x5579FAC0U, 0x9B31BB4AU, 0x96929935U, 0x562556BAU, 0x2BE27FC4U, 0x9C22E4D6U, 0x6F2C5505U,
      0x0BDBDF21U, 0x4C62CB81U, 0x9AF3D17DU, 0x2E2EFE50U, 0x6B457F0AU, 0xE0BEAC61U, 0x3A55EF62U, 0xA386559BU,
      0x86D3D2D4U, 0x8138C51FU, 0x8D893530U, 0x2654B999U, 0x89F57F59U, 0x123E1C2FU, 0xABC30345U, 0x061D761CU,
      0xF1D4E242U, 0x9823F45EU, 0x8C4B5F07U, 0x9EE8DEFCU, 0xB49556E9U, 0xD962CF8AU, 0x0DB408F1U, 0xCAB77682U,
      0x68DDB3F8U, 0xB30EA79DU, 0x8E0DE15EU, 0x8C5D7112U, 0xF3352C39U, 0x5FF6BD24U, 0x3C5C126CU, 0x44387161U,
      0x1FDA836EU, 0xAA1596DCU, 0x8FCF8B69U, 0x34E11677U, 0xCE550589U, 0x94AA6E81U, 0x9A2B19D8U, 0x889271FFU,
      0x81BE16CDU, 0xE554001BU, 0x8A809DECU, 0xA9362ECEU, 0x7C75D999U, 0x89AF5E39U, 0x5F8C2756U, 0x825778E6U,
      0xF6B9265BU, 0xFC4F315AU, 0x8B42F7DBU, 0x118A49ABU, 0x4115F029U, 0x42F38D9CU, 0xF9FB2CE2U, 0x4EFD7878U,
      0x6FB077E1U, 0xD7626299U, 0x89044982U, 0x033FE645U, 0x06B58AF9U, 0xC467FF32U, 0xC813367FU, 0xC0727F9BU,
      0x18B74777U, 0xCE7953D8U, 0x88C623B5U, 0xBB838120U, 0x3BD5A349U, 0x0F3B2C97U, 0x6E643DCBU, 0x0CD87F05U,
      0x88085AE6U, 0x49E14F17U, 0x839A6488U, 0xE3E09176U, 0xB9853498U, 0xFE6D9E42U, 0x982C4D22U, 0xD5F86DA9U,
      0xFF0F6A70U, 0x50FA7E56U, 0x82580EBFU, 0x5B5CF613U, 0x84E51D28U, 0x35314DE7U, 0x3E5B4696U, 0x19526D37U,
      0x66063BCAU, 0x7BD72D95U, 0x801EB0E6U, 0x49E959FDU, 0xC34567F8U, 0xB3A53F49U, 0x0FB35C0BU, 0x97DD6AD4U,
      0x11010B5CU, 0x62CC1CD4U, 0x81DCDAD1U, 0xF1553E98U, 0xFE254E48U, 0x78F9ECECU, 0xA9C457BFU, 0x5B776A4AU,
      0x8F659EFFU, 0x2D8D8A13U, 0x8493CC54U, 0x6C820621U, 0x4C059258U, 0x65FCDC54U, 0x6C636931U, 0x51B26353U,
      0xF862AE69U, 0x3496BB52U, 0x8551A663U, 0xD43E6144U, 0x7165BBE8U, 0xAEA00FF1U, 0xCA146285U, 0x9D1863CDU,
      0x616BFFD3U, 0x1FBBE891U, 0x8717183AU, 0xC68BCEAAU, 0x36C5C138U, 0x28347D5FU, 0xFBFC7818U, 0x1397642EU,
      0x166CCF45U, 0x06A0D9D0U, 0x86D5720DU, 0x7E37A9CFU, 0x0BA5E888U, 0xE368AEFAU, 0x5D8B73ACU, 0xDF3D64B0U,
      0xA00AE278U, 0x5E7EF3ECU, 0xA9E2D0A0U, 0xD67F4138U, 0x28D4C7DFU, 0x16441B82U, 0x03A0A617U, 0x83D02561U,
      0xD70DD2EEU, 0x4765C2ADU, 0xA820BA97U, 0x6EC3265DU, 0x15B4EE6FU, 0xDD18C827U, 0xA5D7ADA3U, 0x4F7A25FFU,
      0x4E048354U, 0x6C48916EU, 0xAA6604CEU, 0x7C7689B3U, 0x521494BFU, 0x5B8CBA89U, 0x943FB73EU, 0xC1F5221CU,
      0x3903B3C2U, 0x7553A02FU, 0xABA46EF9U, 0xC4CAEED6U, 0x6F74BD0FU, 0x90D0692CU, 0x3248BC8AU, 0x0D5F2282U,
      0xA7672661U, 0x3A1236E8U, 0xAEEB787CU, 0x591DD66FU, 0xDD54611FU, 0x8DD55994U, 0xF7EF8204U, 0x079A2B9BU,
      0xD06016F7U, 0x230907A9U, 0xAF29124BU, 0xE1A1B10AU, 0xE03448AFU, 0x46898A31U, 0x519889B0U, 0xCB302B05U,
      0x4969474DU, 0x0824546AU, 0xAD6FAC12U, 0xF3141EE4U, 0xA794327FU, 0xC01DF89FU, 0x6070932DU, 0x45BF2CE6U,
      0x3E6E77DBU, 0x113F652BU, 0xACADC625U, 0x4BA87981U, 0x9AF41BCFU, 0x0B412B3AU, 0xC6079899U, 0x89152C78U,
      0xAED16A4AU, 0x96A779E4U, 0xA7F18118U, 0x13CB69D7U, 0x18A48C1EU, 0xFA1799EFU, 0x304FE870U, 0x50353ED4U,
      0xD9D65ADCU, 0x8FBC48A5U, 0xA633EB2FU, 0xAB770EB2U, 0x25C4A5AEU, 0x314B4A4AU, 0x9638E3C4U, 0x9C9F3E4AU,
      0x40DF0B66U, 0xA4911B66U, 0xA4755576U, 0xB9C2A15CU, 0x6264DF7EU, 0xB7DF38E4U, 0xA7D0F959U, 0x121039A9U,
      0x37D83BF0U, 0xBD8A2A27U, 0xA5B73F41U, 0x017EC639U, 0x5F04F6CEU, 0x7C83EB41U, 0x01A7F2EDU, 0xDEBA3937U,
      0xA9BCAE53U, 0xF2CBBCE0U, 0xA0F829C4U, 0x9CA9FE80U, 0xED242ADEU, 0x6186DBF9U, 0xC400CC63U, 0xD47F302EU,
      0xDEBB9EC5U, 0xEBD08DA1U, 0xA13A43F3U, 0x241599E5U, 0xD044036EU, 0xAADA085CU, 0x6277C7D7U, 0x18D530B0U,
      0x47B2CF7FU, 0xC0FDDE62U, 0xA37CFDAAU, 0x36A0360BU, 0x97E479BEU, 0x2C4E7AF2U, 0x539FDD4AU, 0x965A3753U,
      0x30B5FFE9U, 0xD9E6EF23U, 0xA2BE979DU, 0x8E1C516EU, 0xAA84500EU, 0xE712A957U, 0xF5E8D6FEU, 0x5AF037CDU,
      0xBDBDF21CU, 0x14BCE1BDU, 0xB5C473D0U, 0x866616A7U, 0x4834505DU, 0x15921919U, 0x647E3AD9U, 0xFF6B144AU,
      0xCABAC28AU, 0x0DA7D0FCU, 0xB40619E7U, 0x3EDA71C2U, 0x755479EDU, 0xDECECABCU, 0xC209316DU, 0x33C114D4U,
      0x53B39330U, 0x268A833FU, 0xB640A7BEU, 0x2C6FDE2CU, 0x32F4033DU, 0x585AB812U, 0xF3E12BF0U, 0xBD4E1337U,
      0x24B4A3A6U, 0x3F91B27EU, 0xB782CD89U, 0x94D3B949U, 0x0F942A8DU, 0x93066BB7U, 0x55962044U, 0x71E413A9U,
      0xBAD03605U, 0x70D024B9U, 0xB2CDDB0CU, 0x090481F0U, 0xBDB4F69DU, 0x8E035B0FU, 0x90311ECAU, 0x7B211AB0U,
      0xCDD70693U, 0x69CB15F8U, 0xB30FB13BU, 0xB1B8E695U, 0x80D4DF2DU, 0x455F88AAU, 0x3646157EU, 0xB78B1A2EU,
      0x54DE5729U, 0x42E6463BU, 0xB1490F62U, 0xA30D497BU, 0xC774A5FDU, 0xC3CBFA04U, 0x07AE0FE3U, 0x39041DCDU,
      0x23D967BFU, 0x5BFD777AU, 0xB08B6555U, 0x1BB12E1EU, 0xFA148C4DU, 0x089729A1U, 0xA1D90457U, 0xF5AE1D53U,
      0xB3667A2EU, 0xDC656BB5U, 0xBBD72268U, 0x43D23E48U, 0x78441B9CU, 0xF9C19B74U, 0x579174BEU, 0x2C8E0FFFU,
      0xC4614AB8U, 0xC57E5AF4U, 0xBA15485FU, 0xFB6E592DU, 0x4524322CU, 0x329D48D1U, 0xF1E67F0AU, 0xE0240F61U,
      0x5D681B02U, 0xEE530937U, 0xB853F606U, 0xE9DBF6C3U, 0x028448FCU, 0xB4093A7FU, 0xC00E6597U, 0x6EAB0882U,
      0x2A6F2B94U, 0xF7483876U, 0xB9919C31U, 0x516791A6U, 0x3FE4614CU, 0x7F55E9DAU, 0x66796E23U, 0xA201081CU,
      0xB40BBE37U, 0xB809AEB1U, 0xBCDE8AB4U, 0xCCB0A91FU, 0x8DC4BD5CU, 0x6250D962U, 0xA3DE50ADU, 0xA8C40105U,
      0xC30C8EA1U, 0xA1129FF0U, 0xBD1CE083U, 0x740CCE7AU, 0xB0A494ECU, 0xA90C0AC7U, 0x05A95B19U, 0x646E019BU,
      0x5A05DF1BU, 0x8A3FCC33U, 0xBF5A5EDAU, 0x66B96194U, 0xF704EE3CU, 0x2F987869U, 0x34414184U, 0xEAE10678U,
      0x2D02EF8DU, 0x9324FD72U, 0xBE9834EDU, 0xDE0506F1U, 0xCA64C78CU, 0xE4C4ABCCU, 0x92364A30U, 0x264B06E6U,
  };

  uint32_t ulCrc32;

  if (pBuffer == NULL) {
    ulCrc32 = SUSPICIOUS_CRC_VALUE;
  } else {
    const uint8_t *pbBuffer = pBuffer;
    uint32_t ulIdx = 0U;
    const uint32_t *pulXorCrc0 = &aulCrc32Table[7U];
    const uint32_t *pulXorCrc1 = &aulCrc32Table[6U];
    const uint32_t *pulXorCrc2 = &aulCrc32Table[5U];
    const uint32_t *pulXorCrc3 = &aulCrc32Table[4U];
    const uint32_t *pulXorData4 = &aulCrc32Table[3U];
    const uint32_t *pulXorData5 = &aulCrc32Table[2U];
    const uint32_t *pulXorData6 = &aulCrc32Table[1U];
    const uint32_t *pulXorData7 = &aulCrc32Table[0U];
    uint32_t ulSliceLen;

    ulCrc32 = ~ulInitCrc32;

    /*  Aligned memory access is used below.  To avoid suboptimal
        performance and faults (depending on platform), handle the
        unaligned initial bytes (if any) using the Sarwate algorithm.
    */
    while ((ulIdx < ulLength) && !IS_ALIGNED_PTR(&pbBuffer[ulIdx], REDCONF_ALIGNMENT_SIZE)) {
      ulCrc32 = (ulCrc32 >> 8U) ^ aulCrc32Table[((ulCrc32 ^ pbBuffer[ulIdx]) & 0xFFU) << 3U];

      ulIdx++;
    }

    /*  Round down the length to the nearest multiple of eight.
     */
    ulSliceLen = (((ulLength - ulIdx) >> 3U) << 3U) + ulIdx;

    /*  Compute the CRC in eight byte "slices".  Takes advantage of
        modern processors which can load in parallel from multiple
        memory locations.
    */
    while (ulIdx < ulSliceLen) {
#if REDCONF_ENDIAN_BIG == 1
      ulCrc32 ^= pbBuffer[ulIdx] | ((uint32_t)pbBuffer[ulIdx + 1U] << 8U) | ((uint32_t)pbBuffer[ulIdx + 2U] << 16U) |
                 ((uint32_t)pbBuffer[ulIdx + 3U] << 24U);
#else
      /*  Regarding the cast to (const void *): this is there to placate
          some compilers which emit warnings when a type with lower
          alignment requirements (such as const uint8_t *) is cast to a
          type with higher alignment requirements.  This isn't a concern
          here, since we ensured the pointer was sufficiently aligned.
      */
      ulCrc32 ^= *((const uint32_t *)((const void *)&pbBuffer[ulIdx]));
#endif

      ulCrc32 = pulXorCrc3[((ulCrc32 >> 24U) & 0xFFU) << 3U] ^ pulXorCrc2[((ulCrc32 >> 16U) & 0xFFU) << 3U] ^
                pulXorCrc1[((ulCrc32 >> 8U) & 0xFFU) << 3U] ^ pulXorCrc0[(ulCrc32 & 0xFFU) << 3U] ^
                pulXorData7[pbBuffer[ulIdx + 7U] << 3U] ^ pulXorData6[pbBuffer[ulIdx + 6U] << 3U] ^
                pulXorData5[pbBuffer[ulIdx + 5U] << 3U] ^ pulXorData4[pbBuffer[ulIdx + 4U] << 3U];

      ulIdx += 8U;
    }

    /*  Compute the remaining bytes with the Sarwate algorithm.
     */
    while (ulIdx < ulLength) {
      ulCrc32 = (ulCrc32 >> 8U) ^ aulCrc32Table[((ulCrc32 ^ pbBuffer[ulIdx]) & 0xFFU) << 3U];

      ulIdx++;
    }

    ulCrc32 = ~ulCrc32;
  }

  return ulCrc32;
}

#else

#error "REDCONF_CRC_ALGORITHM must be set to CRC_BITWISE, CRC_SARWATE, or CRC_SLICEBY8"

#endif

/** @brief Compute a CRC32 for a metadata node buffer.

    @param pBuffer  The metadata node buffer for which to compute a CRC.  Must
                    be a block sized buffer.

    @return The CRC of the buffer.
*/

/*  MOCK STARTS */

uint32_t computeCrc32(const uint32_t prevCrc32, const uint8_t *buffer, size_t len) {
  return RedCrc32Update(prevCrc32, buffer, len);
}
