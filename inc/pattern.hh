#ifndef __DMX512USB_SOFTWARE_PATTERN_HH__
#define __DMX512USB_SOFTWARE_PATTERN_HH__

#include <stdint.h>
#include <vector>

namespace dmx512usb_software {

	class Source;
	class Sink {
		public:
			virtual void register_source(Source *s) = 0;
			virtual void remove_source(Source *s) = 0;
	};

	class Block {
		protected:
			/* TODO: more stuff */
			uint8_t value;
		public:
			Block(void);
			uint8_t get_value( /* TODO */ );
	};

	class Sequence {
		friend class Source;
		protected:
			short channel;
			std::vector<Block> bls;
		public:
			Sequence(void);
	};

	class Pattern {
		friend class ActivePattern;
		protected:
			bool abschan;
			std::vector<Sequence> seqs;
		public:
			Pattern(void);
	};

	class ActivePattern;
	class Source {
		protected:
			ActivePattern *ap;
			Sequence *seq;
			unsigned short channel;

		public:
			Source(ActivePattern *pat, Sequence *s, short rel);
			uint8_t get_value( /* TODO */ );
			unsigned short get_channel(void);

	};

	class ActivePattern {
		protected:
			Pattern *pat;
			Sink *sink;
			std::mutex m;
			unsigned short channelrel;
			std::vector<Source*> srcs;
			double localfader;
			unsigned short nactive;

		public:
			ActivePattern(Pattern *p, short channel, Sink *s);
			void set_localfader(double value);
			double get_faderproduct(void);

			void request_removal(Source *s);
			void deactivate(bool immediately = true);



	};

}


#endif
