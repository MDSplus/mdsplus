
CLEANFILES = *.log *.idx *.out *.toc *.aux



SUFFIXES = .pdf .png
.PRECIOUS: %.tex %.pdf %.png


# //////////////////////////////////////////////////////////////////////////// #
# // GENERATE IMAGES //

IMAGES = \
         mdsip_login.png \
         tc_open.png \
         tc_value.png

#### implicit rules ####


.tex.pdf:
	pdflatex $<
        
.pdf.png:
	convert $< $@
        


PHONY: images
images: ${IMAGES}


if DX_COND_pdf
all: images
endif DX_COND_pdf


# //////////////////////////////////////////////////////////////////////////// #


