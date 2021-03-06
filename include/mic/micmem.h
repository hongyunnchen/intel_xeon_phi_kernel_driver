/*
 * Copyright (C) 2014 PathScale Inc. All Rights Reserved.
 */

/* Micmem is a collection of functions allowing fast DMA access to card memory.
 * They are used best without full MPSS stack running, and without an OS present
 * on the device side. By design, these functions reserve several DMA channels
 * for their exclusive use. Using this functionality together with a full MPSS
 * stack may lead to exhaustion of available DMA channels.
 * This code has been tested to work on Knight's Corner devices only.
 * This basic library is not threadsafe.
 */

#ifndef __MICMEM_H__
#define __MICMEM_H__

#ifdef CONFIG_MK1OM

#include "mic_common.h"
#include "micmem_const.h"
/** micmem_ctx:
 * Memory context for a device.
 * Holds device-specific data used to perform DMA operations using the device.
 * Separate channels for both directions used for full duplex operation.
 */
struct micmem_ctx {
	mic_ctx_t *mic_ctx;
	struct dma_channel *h2d_ch; /* Channel reserved for host2dev */
	struct dma_channel *d2h_ch; /* Channel reserved for dev2host */
	/* secondary channels for dual channel */
	struct dma_channel *h2d_ch2;
	struct dma_channel *d2h_ch2;
};

/** dma_mem_range:
 * Describes a set of pinned host pages, mapped to device memory.
 * TODO: remember which device (mem_ctx)
 */
struct dma_mem_range { /* TODO: find best alignment */
	dma_addr_t *dma_addr; /* indexed by contiguous chunk number */
	int *num_pages; /* indexed by contiguous chunk number */
	int nr_contig_chunks;
	struct scif_pinned_pages *pinned_pages;
	uint64_t size;
};


int micmem_get_mem_ctx(mic_ctx_t *mic_ctx, struct micmem_ctx *mem_ctx);
void micmem_destroy_mem_ctx(struct micmem_ctx *mem_ctx);
int micmem_pin_range(void *host_vm, uint64_t len, struct scif_pinned_pages **pinned_pages);
int micmem_map_range(mic_ctx_t *mic_ctx, struct scif_pinned_pages *pinned_pages, uint64_t offset, uint64_t len, struct dma_mem_range **out_range);
void micmem_unmap_range(mic_ctx_t *mic_ctx, struct dma_mem_range *mem_range);
void micmem_unpin_range(struct scif_pinned_pages *pinned_pages);
int micmem_dev2host(struct micmem_ctx *mem_ctx, struct dma_mem_range *dest_mem_range, uint64_t range_offset, uint64_t source_dev, uint64_t size, int flags);
int micmem_host2dev(struct micmem_ctx *mem_ctx, uint64_t dest_dev, struct dma_mem_range *src_mem_range, uint64_t range_offset, uint64_t size, int flags);

#endif /* CONFIG_MK1OM */

#endif /* __MICMEM_H__ */
