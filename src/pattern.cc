#include "pattern.hh"

namespace dmx512usb_software {

	// {{{ Block class
	Block::Block(void) :
		value(0xff) {}

	uint8_t Block::get_value( /* TODO */) {
		return value;
	}
	// }}}


	// {{{ Sequence class
	// TODO: default constructor should probably not push an
	// element on the blockvector
	Sequence::Sequence(void) :
			channel(0),
			bls(std::vector<Block>(1, Block())) {};

	// }}}


	// {{{ Pattern class
		Pattern::Pattern(void) :
			abschan(false),
			seqs(std::vector<Sequence>(1, Sequence())) {};
	// }}}


	// {{{ Source class
	Source::Source(ActivePattern *p, Sequence *s, short rel) :
			ap(p),
			seq(s),
			channel(seq->channel+rel) {}

	uint8_t Source::get_value( /* TODO */ ) {
		uint8_t value = seq->bls.begin()->get_value( /* TODO */ );
		/*	TODO: if source is at end of sequence,
			request the ap to remove it, or restart it,
			depending on if it is a oneshot or a loop */
		return value * ap->get_faderproduct();
	}

	unsigned short Source::get_channel(void) {
		return channel;
	}
	// }}}


	// {{{ ActivePattern class
	ActivePattern::ActivePattern(Pattern *p, short channel, Sink *s) :
			pat(p),
			sink(s),
			channelrel(p->abschan?0:channel) {
		nactive = p->seqs.size();
		for (std::vector<Sequence>::iterator seqit=p->seqs.begin();
				seqit!=p->seqs.end(); ++seqit) {
			Source *src = new Source(this, &(*seqit), channelrel);
			srcs.push_back(src);
			sink->register_source(src);
		}
	}

	void ActivePattern::set_localfader(double value) {
		localfader = value;
	}

	double ActivePattern::get_faderproduct(void) {
		return masterfader * localfader;
	}

	void ActivePattern::request_removal(Source *s) {
		sink->remove_source(s);
		if (--nactive == 0) {
			/* TODO signal something upstream to the controls */
		}
	}

	void ActivePattern::deactivate(bool immediately) {
		if (immediately) {
			for (std::vector<Source*>::iterator svit=srcs.begin();
					svit!=srcs.end(); ++svit) {
				request_removal(*svit);
			}
		}
		else {
			/* TODO set the running pattern to oneshot mode so that it will be removed once completed */
		}
	}
	// }}}


}
