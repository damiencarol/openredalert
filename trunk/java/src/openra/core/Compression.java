// ======================================
// Scanner's Java - File format utilities
// ======================================

package openra.core;

import java.nio.ByteBuffer;

/**
 * Utility class with methods to deal with the several file formats used by both
 * Red Alert and Tiberium Dawn.  The methods here are mainly used to decode,
 * decrypt, or decompress the files used, as well as conversion to turn the new
 * raw data into forms more easily used by OpenGL, and OpenAL.<br>
 * <br>
 * Encoding was not normally in the scope of this program, but a request for
 * some modern SHP file encoding led to it's creation.  That, and current SHP
 * encoders have restrictions which have since been loosened through .exe hacks
 * and other advanced editing, or they just don't work as advertised.<br>
 * <br>
 * Much of the C&C-specific methods (Format20, Format40, Format80,
 * 16bitIMAADPCM) have been adapted from code written by Vladan Bato.  Credit
 * also goes to Asatur V. Nazarian for the 8bitWSADPCM code.
 *
 * @author Emanuel Rabina
 */
public class Compression 
{

	/**
	 * Hidden default constructor, as this class is only ever meant to be used
	 * statically.
	 */
	private Compression() 
	{
	}

	/**
	 * Decodes Format2-encoded data.
	 *
	 * @param source Original encoded data.
	 * @param dest	 Buffer to store the decoded data.
	 */
	public static void decodeFormat2(ByteBuffer source, ByteBuffer dest) 
	{
		Format2.decode(source, dest);
	}

	/**
	 * Decodes Format20-encoded data.<br>
	 * <br>
	 * In the context of images, Format20 is just Format40 XOR'ed over the
	 * previous image.  This method is merely a call to the Format40 decoding
	 * method, but kept so that the differences between format types can be
	 * upheld.
	 *
	 * @param source Original encoded data.
	 * @param base	 Bytes from the frame the new data is based upon.
	 * @param dest	 Buffer to store the decoded data.
	 */
	public static void decodeFormat20(ByteBuffer source, ByteBuffer base, ByteBuffer dest) 
	{
		decodeFormat40(source, base, dest);
	}

	/**
	 * Decodes Format40-encoded data.
	 *
	 * @param source Original encoded data.
	 * @param base	 Bytes from the frame the new data is based upon.
	 * @param dest	 Buffer to store the decoded data.
	 */
	public static void decodeFormat40(ByteBuffer source, ByteBuffer base, ByteBuffer dest) 
	{
		Format40.decode(source, base, dest);
	}

	/**
	 * Decodes Format80-encoded data.
	 *
	 * @param source Original compressed image bytes.
	 * @param dest	 Buffer to store the uncompressed image bytes.
	 */
	public static void decodeFormat80(ByteBuffer source, ByteBuffer dest) 
	{
		Format80.decode(source, dest);
	}

	/**
	 * Encodes a series of bytes using the Format2 algorithm.
	 *
	 * @param source Original raw data.
	 * @param slices Number of rows to run the encoding on.  If this is image
	 * 				 data, then this is the height of the image.
	 * @param dest   Buffer to store the encoded data.
	 */
	public static void encodeFormat2(ByteBuffer source, int slices, ByteBuffer dest) 
	{
		Format2.encode(source, slices, dest);
	}

	/**
	 * Encodes a series of bytes using the Format40 algorithm.
	 *
	 * @param source Original raw data.
	 * @param base	 Bytes from the frame the new data must be based upon.
	 * @param dest   Buffer to store the encoded data.
	 */
	public static void encodeFormat40(ByteBuffer source, ByteBuffer base, ByteBuffer dest) 
	{
		Format40.encode(source, base, dest);
	}

	/**
	 * Encodes a series of bytes using the Format80 algorithm.
	 *
	 * @param source Original raw image bytes.
	 * @param dest   Buffer to store the encoded image bytes.
	 */
	public static void encodeFormat80(ByteBuffer source, ByteBuffer dest) {

		Format80.encode(source, dest);
	}

	/**
	 * Inner class containing parts related to the Format2 compression scheme.
	 */
	private static class Format2 {

		private static byte CMD_FILL     = 0;
		private static byte CMD_FILL_VAL = 0;

		/**
		 * Decodes Format2-encoded data.<br>
		 * <br>
		 * Format2 is used for Dune 2 SHP files and is a simple compression
		 * technique which can be deciphered as follows:<br>
		 * 1) 0 c = Fill the next c bytes with 0.<br>
		 * 2) v = Write v.<br>
		 *
		 * @param source Original encoded data.
		 * @param dest	 Buffer to store the decoded data.
		 */
		private static void decode(ByteBuffer source, ByteBuffer dest) {

			while (source.hasRemaining()) {
				byte command = source.get();

				// Fill 0s
				if (command == CMD_FILL) {
					int count = source.get() & 0xff;
					while (count-- > 0) {
						dest.put(CMD_FILL_VAL);
					}
				}
				// Write direct value
				else {
					dest.put(command);
				}
			}
			dest.flip();
		}

		/**
		 * Encodes a series of bytes using the Format2 algorithm.<br>
		 * <br>
		 * Format2 is used for Dune 2 SHP files and is a simple compression
		 * technique which is used as follows:<br>
		 * 1) Series of 0s = write a 0 followed by how many bytes of 0 there were. A
		 * 					 maximum of <code>slices</code> bytes can be described
		 * 					 by this method.<br>
		 * 2) Any non-0 byte = write that non-0 byte.
		 *
		 * @param source Original raw image bytes.
		 * @param slices Number of rows to run the encoding on.  This is usually the
		 * 				 height of the original image.
		 * @param dest   Buffer to store the encoded image bytes.
		 */
		private static void encode(ByteBuffer source, int slices, ByteBuffer dest) {

			int count;
			int limit = source.limit() / slices;

			while (source.hasRemaining()) {
				count = 0;

				for (int pixel = 0; pixel < limit; pixel++) {
					byte value = source.get();

					// Read 0
					if (value == CMD_FILL_VAL) {
						count++;
					}
					// Read non-0
					else {

						// End of series of 0s, describe series
						if (count > 0) {
							dest.put(new byte[]{ 0, (byte)count });
							count = 0;
						}
						// Write non-0 value
						dest.put(value);
					}
				}
				// Finish remaining series of 0s (if any)
				if (count > 0) {
					dest.put(new byte[]{ CMD_FILL, (byte)count });
				}
			}
			source.rewind();
			dest.flip();
		}
	}

	/**
	 * Inner class containing parts related to the Format40 compression scheme.
	 */
	private static class Format40 {

		// Small skip command
		private static byte CMD_SKIP_S           = (byte)0x80;		// 10000000
		private static int  CMD_SKIP_S_MAX       = 63;				// 01111111, 0x7f
		private static int  CMD_SKIP_S_THRESHOLD = 0;

		// Large skip command
		private static byte  CMD_SKIP_L1          = (byte)0x80;		// 10000000
		private static short CMD_SKIP_L2          = (short)0x0000;	// 00000000 00000000
		private static int   CMD_SKIP_L_MAX       = 32767;			// 01111111 11111111, 0x7fff
//		private static int   CMD_SKIP_L_THRESHOLD = 2;

		// Small fill command
		private static byte CMD_FILL_S           = (byte)0x00;		// 00000000
		private static int  CMD_FILL_S_MAX       = 255;				// 11111111, 0xff
		private static int  CMD_FILL_S_THRESHOLD = 2;

		// Large fill command
		private static byte  CMD_FILL_L1          = (byte)0x80;		// 10000000
		private static short CMD_FILL_L2          = (short)0xc000;	// 11000000 00000000
		private static int   CMD_FILL_L_MAX       = 16383;			// 00111111 11111111, 0x3fff
//		private static int   CMD_FILL_L_THRESHOLD = 3;

		// Small XOR command
		private static byte CMD_XOR_S           = (byte)0x00;		// 00000000
		private static byte CMD_XOR_S_MAX       = 63;				// 01111111, 0x7f
//		private static int  CMD_XOR_S_THRESHOLD = 0;

		// Large XOR command
		private static byte  CMD_XOR_L1          = (byte)0x80;		// 10000000
		private static short CMD_XOR_L2          = (short)0x8000;	// 10000000 00000000
		private static int   CMD_XOR_L_MAX       = 16383;			// 00111111 11111111, 0x3fff
//		private static int   CMD_XOR_L_THRESHOLD = 2;

		/**
		 * Decodes Format40-encoded data.<br>
		 * <br>
		 * The documentation of Format40 defines 6 special commands used
		 * depending upon the head bits of the byte read.  Using a notation
		 * found in XCCU, they are as follows:<br>
		 * 1) 00000000 c v = Xor next c bytes with v.<br>
		 * 2) 0ccccccc = Xor the next c bytes from source with those in base.<br>
		 * 3) 10000000 0c c = Skip the next c bytes.<br>
		 * 4) 10000000 10c c = Xor the next c bytes from source with those in base.<br>
		 * 5) 10000000 11c c v = Xor the next c bytes with v.<br>
		 * 6) 1ccccccc = Skip the next c bytes.
		 *
		 * @param source Original encoded data.
		 * @param base	 Bytes from the frame the new data is based upon.
		 * @param dest	 Buffer to store the decoded data.
		 */
		private static void decode(ByteBuffer source, ByteBuffer base, ByteBuffer dest) {

			int count;

			while (true) {
				byte command = source.get();

				// b7 = 0
				if ((command & 0x80) == 0) {

					// Command #1 - small XOR base with value
					if (command == 0) {
						count = source.get() & 0xff;
						byte fill = source.get();
						while (count-- > 0) {
							dest.put((byte)(base.get() ^ fill));
						}
					}
					// Command #2 - small XOR source with base for count
					else {
						count = command;
						while (count-- > 0) {
							dest.put((byte)(source.get() ^ base.get()));
						}
					}
				}
				// b7 = 1
				else {
					count = command & 0x7f;

					// b6-0 = 0
					if (count == 0) {
						count = source.getShort() & 0xffff;
						command = (byte)(count >>> 8);

						// b7 of next byte = 0
						if ((command & 0x80) == 0) {

							// Finished decoding
							if (count == 0) {
								break;
							}

							// Command #3 - large copy base to dest for count
							while (count-- > 0) {
								dest.put(base.get());
							}
						}
						// b7 of next byte = 1
						else {
							count &= 0x3fff;

							// Command #4 - large XOR source with base for count
							if ((command & 0x40) == 0) {
								while (count-- > 0) {
									dest.put((byte)(source.get() ^ base.get()));
								}
							}
							// Command #5 - large XOR base with value
							else {
								byte fill = source.get();
								while (count-- > 0) {
									dest.put((byte)(base.get() ^ fill));
								}
							}
						}
					}
					// b6-0 != 0
					else {

						// Command #6 - small copy base to dest for count
						while (count-- > 0) {
							dest.put(base.get());
						}
					}
				}
			}
			source.rewind();
			base.rewind();
			dest.flip();
		}

		/**
		 * Encodes a series of bytes using the Format40 algorithm.<br>
		 * <br>
		 * All Format40 instructions are supported:<br><i>
		 * 1) 00000000 c v = Xor next c bytes with v.<br>
		 * 2) 0ccccccc = Xor the next c bytes from source with those in base.<br>
		 * 3) 10000000 0c c = Skip the next c bytes.<br>
		 * 4) 10000000 10c c = Xor the next c bytes from source with those in base.<br>
		 * 5) 10000000 11c c v = Xor the next c bytes with v.<br>
		 * 6) 1ccccccc = Skip the next c bytes.</i>
		 *
		 * @param source Original raw data.
		 * @param base	 Bytes from the frame the new data is based upon.
		 * @param dest   Buffer to store the encoded data.
		 */
		private static void encode(ByteBuffer source, ByteBuffer base, ByteBuffer dest) {

			// Encode the source
			while (source.hasRemaining()) {

				// Select the method that provdes the best results for the coming bytes
				int skiplength = isCandidateForSkipCommand(source, base);
				int filllength = isCandidateForFillCommand(source, base);
				int xorlength  = isCandidateForXORCommand(source, base);

				int bestmethod = Math.max(skiplength, Math.max(filllength, xorlength));

				// Either small or large skip
				if (bestmethod == skiplength) {

					// Command #6 - small skip
					if (skiplength <= CMD_SKIP_S_MAX) {
						dest.put((byte)(CMD_SKIP_S | skiplength));
					}

					// Command #3 - large skip
					else {
						dest.put(CMD_SKIP_L1);
						dest.putShort((short)(CMD_SKIP_L2 | skiplength));
					}

					source.position(source.position() + skiplength);
					base.position(base.position() + skiplength);
				}

				// Either small or large XOR fill
				else if (bestmethod == filllength) {
					byte xorfillval = (byte)(source.get() ^ base.get());

					// Command #1 - small XOR fill
					if (filllength <= CMD_FILL_S_MAX) {
						dest.put(CMD_FILL_S);
						dest.put((byte)filllength);
					}

					// Command #5 - large XOR fill
					else {
						dest.put(CMD_FILL_L1);
						dest.putShort((short)(CMD_FILL_L2 | filllength));
					}

					dest.put(xorfillval);
					source.position(source.position() - 1 + filllength);
					base.position(base.position() - 1 + filllength);
				}

				// Either small or large XOR
				else {

					// Command #2 - small XOR
					if (xorlength <= CMD_XOR_S_MAX) {
						dest.put((byte)(CMD_XOR_S | xorlength));
					}

					// Command #4 - large XOR
					else {
						dest.put(CMD_XOR_L1);
						dest.putShort((short)(CMD_XOR_L2 | xorlength));
					}

					while (xorlength-- > 0) {
						dest.put((byte)(source.get() ^ base.get()));
					}
				}
			}

			// SHP data must be closed by the large skip command with a length of 0
			dest.put(CMD_SKIP_L1);
			dest.putShort(CMD_SKIP_L2);

			source.rewind();
			base.rewind();
			dest.flip();
		}

		/**
		 * Performs a test on the data to determine whether it is a good
		 * candidate for the Format40 skip commands:<br><i>
		 * 3) 10000000 0c c = Skip the next c bytes.<br>
		 * 6) 1ccccccc = Skip the next c bytes.</i><br>
		 * <br>
		 * Buffer positions are restored after this method is through, and no
		 * alterations are made to the source data.
		 *
		 * @param source Original raw data.
		 * @param base	 Data the Format40 is based off.
		 * @return An integer value indicating that this number of bytes can be
		 * 		   encoded using the skip command, or 0 if the following bytes
		 * 		   aren't good for the skip command.
		 */
		private static int isCandidateForSkipCommand(ByteBuffer source, ByteBuffer base) {

			// Retain current position
			source.mark();
			base.mark();

			// Find out how many bytes there are in common to skip
			int candidatelength = 0;
			while (source.hasRemaining() && candidatelength < CMD_SKIP_L_MAX) {
				if (source.get() != base.get()) {
					break;
				}
				candidatelength++;
			}

			// Reset prior position
			source.reset();
			base.reset();

			// Evaluate skip command candidacy
			return candidatelength > CMD_SKIP_S_THRESHOLD ? candidatelength : 0;
		}

		/**
		 * Performs a test on the data to determine whether it is a good
		 * candidate for the Format40 XOR fill commands:<br><i>
		 * 1) 00000000 c v = Xor next c bytes with v.<br>
		 * 5) 10000000 11c c v = Xor the next c bytes with v.</i><br>
		 * <br>
		 * Buffer positions are restored after this method is through, and no
		 * alterations are made to the source data.
		 *
		 * @param source Original raw data.
		 * @param base	 Data the Format40 is based off.
		 * @return An integer value indicating that this number of bytes can be
		 * 		   encoded using the fill command, or 0 if the following bytes
		 * 		   aren't good for the fill command.
		 */
		private static int isCandidateForFillCommand(ByteBuffer source, ByteBuffer base) {

			// Retain current position
			source.mark();
			base.mark();

			// Find out how many similar bytes can be XOR'ed over contiguous base data
			int candidatelength = 1;
			byte sourcebyte = source.get();
			byte basebyte   = base.get();

			while (source.hasRemaining() && candidatelength < CMD_FILL_L_MAX) {
				if (source.get() != sourcebyte || base.get() != basebyte) {
					break;
				}
				candidatelength++;
			}

			// Reset prior position
			source.reset();
			base.reset();

			// Evaluate skip command candidacy
			return candidatelength > CMD_FILL_S_THRESHOLD ? candidatelength : 0;
		}

		/**
		 * Performs a test on the data to determine whether it is a good
		 * candidate for the Format40 XOR commands:<br><i>
		 * 2) 0ccccccc = Xor the next c bytes from source with those in base.<br>
		 * 4) 10000000 10c c = Xor the next c bytes from source with those in base.</i><br>
		 * <br>
		 * Buffer positions are restored after this method is through, and no
		 * alterations are made to the source data.
		 *
		 * @param source Original raw data.
		 * @param base	 Data the Format40 is based off.
		 * @return An integer value indicating that this number of bytes can be
		 * 		   encoded using the XOR command.  Unlike other methods, this
		 * 		   check always returns a positive number.
		 */
		private static int isCandidateForXORCommand(ByteBuffer source, ByteBuffer base) {

			// Retain current position
			source.mark();
			base.mark();

			// Find out how many dissimilar bytes can be encoded with the XOR command
			int candidatelength = 1;
			int runlength = 1;
			byte lastsourcebyte = source.get();
			byte lastbasebyte   = base.get();

			while (source.hasRemaining() && candidatelength < CMD_XOR_L_MAX) {
				byte nextsourcebyte = source.get();
				byte nextbasebyte   = base.get();

				if (nextsourcebyte == lastsourcebyte && nextbasebyte == lastbasebyte) {
					runlength++;
					if (runlength == 2) {
						candidatelength -= runlength - 2;
						break;
					}
				}
				else {
					runlength = 1;
				}
				candidatelength++;
				lastsourcebyte = nextsourcebyte;
				lastbasebyte   = nextbasebyte;
			}

			// Reset prior position
			source.reset();
			base.reset();

			// Evaluate skip command candidacy
			return candidatelength;
		}
	}

	/**
	 * Inner class containing parts related to the Format80 compression scheme.
	 */
	private static class Format80 {

		// Transfer command
		private static byte CMD_TRANSFER       = (byte)0x80;	// 10000000
		private static int  CMD_TRANSFER_MAX   = 63;			// 00111111, 0x3f

		// Small copy command
		private static byte CMD_COPY_S           = (byte)0xc0;	// 11000000
		private static int  CMD_COPY_S_MAX       = 64;			// -3 = 00111101, 0x3d
		private static int  CMD_COPY_S_THRESHOLD = 3;

		// Large copy command
		private static byte CMD_COPY_L           = (byte)0xff;	// 11111111
		private static int  CMD_COPY_L_MAX       = 65535;		// 11111111 11111111, 0xffff
//		private static int  CMD_COPY_L_THRESHOLD = 4;

		// Colour command
		private static byte CMD_FILL           = (byte)0xfe;	// 11111110
		private static int  CMD_FILL_MAX       = 65535;			// 11111111 11111111, 0xffff
		private static int  CMD_FILL_THRESHOLD = 3;

		/**
		 * Decodes Format80-encoded data.<br>
		 * <br>
		 * The documentation of Format80 defines 5 special commands used
		 * depending upon the head bits of the byte read.  Using a notation
		 * found in XCCU, they are as follows:<br>
		 * 1) 0cccpppp p = Copy c + 3 bytes from dest.pos - p to dest.pos.<br>
		 * 2) 10cccccc = Copy next c bytes from source to dest.<br>
		 * 3) 11cccccc p p = Copy c + 3 bytes from p.<br>
		 * 4) 11111110 c c v = Write c bytes with v.<br>
		 * 5) 11111111 c c p p = Copy c bytes from p.
		 *
		 * @param source Original compressed data.
		 * @param dest	 Buffer to store the uncompressed data.
		 */
		private static void decode(ByteBuffer source, ByteBuffer dest) {

			int count;
			int copypos;

			while (true) {
				byte command = source.get();
				/*int cmd = command;
				if (cmd<0) cmd+=256; 
				System.out.println("DEBUG: command=" + cmd);*/

				// b7 = 0
				if ((command & 0x80) == 0) {
					// Command #1
					count = (command >>> 4) + 3;
					copypos = ((command & 0x0f) << 8) | (source.get() & 0xff);
					copypos = dest.position() - copypos;
					while (count-- > 0) {
						/*int cp = dest.get(copypos);
						if (cp <0) cp+=256;
						System.out.println("DEBUG: copy " + cp + "");*/
						dest.put(dest.get(copypos++));
					}
				}
				// b7 = 1
				else {
					count = command & 0x3f;

					// b6 = 0
					if ((command & 0x40) == 0) {

						// Finished decoding
						if (count == 0) {
							break;
						}

						// Command #2
						while (count-- > 0) {
							byte data = source.get();
							/*int cp = data;if (cp <0) cp+=256;
							System.out.println("DEBUG: copy " + cp + "");*/
							dest.put(data);
						}
					}
					// b6 = 1
					else {

						// Command #3
						if (count < 0x3e) {
							count += 3;
							copypos = source.getShort() & 0xffff;
							while (count-- > 0) {
								byte data = dest.get(copypos++);
								/*int cp = data;if (cp <0) cp+=256;
								System.out.println("DEBUG: copy " + cp + "");	*/							
								dest.put(data);
							}
						}
						// Command #4
						else if (count == 0x3e) {
							count = source.getShort() & 0xffff;
							byte fill = source.get();
							while (count-- > 0) {
								dest.put(fill);
							}
						}
						// Command #5
						else {
							count = source.getShort() & 0xffff;
							copypos = source.getShort() & 0xffff;
							/*System.out.println("DEBUG: count=" + count + " pos=" + copypos);	*/							
							while (count-- > 0) {
								dest.put(dest.get(copypos++));
							}
						}
					}
				}
			}
			source.rewind();
			dest.flip();
		}

		/**
		 * Encodes a series of bytes using the Format80 algorithm.<br>
		 * <br>
		 * Current Format80 instructions implemented:<br><i>
		 * 2) 10cccccc = Copy next c bytes from source to dest.<br>
		 * 3) 11cccccc p p = Copy c + 3 bytes from p.<br>
		 * 4) 11111110 c c v = Write c bytes with v.<br>
		 * 5) 11111111 c c p p = Copy c bytes from p.</i>
		 *
		 * @param source Original raw data.
		 * @param dest   Buffer to store the encoded data.
		 */
		private static void encode(ByteBuffer source, ByteBuffer dest) {

			// String representation of the source
			byte[] sourcebytes = new byte[source.limit()];
			source.get(sourcebytes).rewind();

			// Format80 data must be opened by the transfer command w/ a length of 1
			dest.put((byte)(CMD_TRANSFER | 1));
			dest.put(source.get());

			// Encode the source
			while (source.hasRemaining()) {

				// Select the method that provdes the best results for the coming bytes
				int[] copypart = isCandidateForCopyCommand(source);
				int filllength = isCandidateForFillCommand(source);
				int xferlength = isCandidateForTransferCommand(source);

				int bestmethod = Math.max(copypart[0], Math.max(filllength, xferlength));

				// Command #4 - run-length encoding, aka: fill
				if (bestmethod == filllength) {
					byte colourval = source.get();

					dest.put(CMD_FILL);
					dest.putShort((short)filllength);
					dest.put(colourval);

					source.position(source.position() - 1 + filllength);
				}

				// Either small or large copy
				else if (bestmethod == copypart[0]) {

					// Command #3 - small copy
					if (copypart[0] <= CMD_COPY_S_MAX) {
						dest.put((byte)(CMD_COPY_S | (copypart[0] - 3)));
						dest.putShort((short)copypart[1]);
					}

					// Command #5 - large copy
					else {
						dest.put(CMD_COPY_L);
						dest.putShort((short)copypart[0]);
						dest.putShort((short)copypart[1]);
					}

					source.position(source.position() + copypart[0]);
				}

				// Command #2 - straight transfer of bytes from source to dest
				else {
					byte[] xferbytes = new byte[xferlength];
					source.get(xferbytes);

					dest.put((byte)(CMD_TRANSFER | xferlength));
					dest.put(xferbytes);
				}
			}

			// SHP data must be closed by the transfer command w/ a length of 0
			dest.put(CMD_TRANSFER);

			source.rewind();
			dest.flip();
		}

		/**
		 * Performs a test on the data to determine whether it is a good
		 * candidate for either Format80 copy command:<br><i>
		 * 3) 11cccccc p p = Copy c + 3 bytes from p.<br>
		 * 5) 11111111 c c p p = Copy c bytes from p.</i><br>
		 * <br>
		 * Buffer positions are restored after this method is through, and no
		 * alterations are made to the source data.
		 *
		 * @param source Original raw data.
		 * @return The first element indicates that this number of bytes can be
		 * 		   encoded using the copy command, or 0 if the following bytes
		 * 		   aren't good for the copy command.  The second integer is the
		 * 		   position in the source buffer that these bytes occur, if
		 * 		   the first is not 0.
		 */
		private static int[] isCandidateForCopyCommand(ByteBuffer source) {

			// Retain current position
			source.mark();

			// Copy of the bytes read thus far
			ByteBuffer sourcecopy = source.duplicate();
			sourcecopy.limit(Math.min(source.position(), CMD_COPY_L_MAX));
			sourcecopy.position(0);

			int candidatelength   = 0;
			int candidateposition = -1;

			// Search for instances of the remaining bytes in the source so far
			while (source.hasRemaining() && sourcecopy.hasRemaining()) {
				int copypos = sourcecopy.position();

				// Potential match
				int runlength = 0;
				while (source.hasRemaining() && sourcecopy.hasRemaining()) {
					if (source.get() == sourcecopy.get()) {
						runlength++;
					}
					else {
						break;
					}
				}

				// Update candidate length and position?
				if (runlength > candidatelength) {
					candidatelength = runlength;
					candidateposition = copypos;
				}

				source.reset();
			}

			// Reset prior position
			source.reset();

			// Evaluate copy command candidacy
			return new int[]{ candidatelength > CMD_COPY_S_THRESHOLD ? candidatelength : 0, candidateposition };
		}

		/**
		 * Performs a test on the data to determine whether it is a good
		 * candidate for the Format80 fill command (similar to run-length
		 * encoding):<br><i>
		 * 4) 11111110 c c v = Write c bytes with v.</i><br>
		 * <br>
		 * Buffer positions are restored after this method is through, and no
		 * alterations are made to the source data.
		 *
		 * @param source Original raw data.
		 * @return An integer value indicating that this number of bytes can be
		 * 		   encoded using the fill command, with 0 indicating that the
		 * 		   following bytes aren't good for the fill command.
		 */
		private static int isCandidateForFillCommand(ByteBuffer source) {

			// Retain current position
			source.mark();

			// Find out how many bytes ahead have the same value as the starting byte
			int candidatelength = 1;
			byte fillbyte = source.get();

			while (source.hasRemaining() && candidatelength < CMD_FILL_MAX) {
				if (fillbyte != source.get()) {
					break;
				}
				candidatelength++;
			}

			// Reset prior position
			source.reset();

			// Evaluate fill command candidacy
			return candidatelength > CMD_FILL_THRESHOLD ? candidatelength : 0;
		}

		/**
		 * Performs a test on the data to determine whether it is a good
		 * candidate for the Format80 transfer command:<br><i>
		 * 2) 10cccccc = Copy next c bytes from source to dest.</i><br>
		 * <br>
		 * Buffer positions are restored after this method is through, and no
		 * alterations are made to the source data.
		 *
		 * @param source Original raw data.
		 * @return An integer value indicating that this number of bytes can be
		 * 		   encoded using the transfer command.  Unlike other methods,
		 * 		   this test always returns a positive result.
		 */
		private static int isCandidateForTransferCommand(ByteBuffer source) {

			// Retain current position
			source.mark();

			// Find out the longest stretch of dissimilar bytes
			int candidatelength = 1;
			int runlength = 1;
			byte lastbyte = source.get();

			while (source.hasRemaining() && candidatelength < CMD_TRANSFER_MAX) {
				byte nextbyte = source.get();
				if (nextbyte == lastbyte) {
					runlength++;
					if (runlength > CMD_FILL_THRESHOLD) {
						candidatelength -= runlength - 2;
						break;
					}
				}
				else {
					runlength = 1;
				}
				candidatelength++;
				lastbyte = nextbyte;
			}

			// Reset prior position
			source.reset();

			// Transfer command candidacy is always valid
			return candidatelength;
		}
	}
}
