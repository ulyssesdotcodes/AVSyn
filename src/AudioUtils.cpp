#include "AudioUtils.h"
#include "cinder\audio\audio.h"

#include <vector>

vector<float> audioUtils::eqs(vector<float> buffer, int binCount) {
	vector<float> bins(binCount);
	int binSize = buffer.size() / binCount;

	for (vector<float>::size_type i = 0; i < buffer.size(); i++) {
		int bin = i / binSize;

		// Just discard the last one if it fits perfectly.
		if(bin < bins.size()) {
			bins[bin] += ci::audio::linearToDecibel(buffer[i]);
		}
	}

	for (vector<float>::iterator it = bins.begin(); it != bins.end(); ++it) {
		// 100.0f to account for linear to decibal
		*it = *it / (binSize * 100.0f);
	}

	return bins;
}
