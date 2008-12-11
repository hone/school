class ImageEntry < Struct.new( :image, :chosen_face ); end

class Eigenface < Shoes
  WIDTH = 800
  HEIGHT = 600

  IMAGE_DIR = "images"

  url '/', :index
  url '/next_image', :next_image

  def index
    @index = -2
    @images = Array.new
    @faces = Hash.new
    draw_main_window
  end

  def draw_main_window
    self.clear
    stack do
      flow do
        button( "load training images" ) { open_folder( @train_folder ) }
        button( "load test images" ) { open_folder( @test_folder ) }
        button( "set value" ) do
          set_value
          next_image( 1 )
        end
        button( "previous image" ) { next_image( -1 ) }
        button( "next image" ) { next_image( 1 ) }
        para "#{@index + 1} / #{@images.size} images"
      end
      para( "filename: #{@image}" ) if @image
      flow do
        stack do
          para "Face"
          @face_number_entry = edit_box :height => 20, :width => 40
        end
        image @image if @image
      end
    end
  end

  def set_value
    @face_number_entry.text
  end

  def extract_faces( image )
    filename = image.split( "/" ).last
    filename_parts = filename.split('.')
    extension = filename_parts[1]
    output_filename = "#{IMAGE_DIR}/#{filename_parts[0]}_face.#{extension}"
    debug( "extract faces: #{output_filename}" )

    system( "./face_detect #{image} #{output_filename}" )
    files = Dir["#{output_filename.split('.')[0]}*.#{extension}"]

    [output_filename] + files
  end

  def open_folder( folder )
    last_index = @images.size
    # pull images
    folder = ask_open_folder
    current_images = Dir["#{folder}/*.jpg"]
    if current_images.empty?
      current_images = Dir["#{folder}/*.png"]
    end
    # extract faces
    current_images.collect! do |image|
      filenames = extract_faces( image )
      filename = filenames.shift
      @faces[filename] = filenames

      ImageEntry.new( filename, nil )
    end

    @images += current_images
    debug( @images.inspect )
    @index = last_index + 1

    self.clear
    draw_main_window
  end

  def next_image( increment )
    debug( "next image index: #{@index}" )
    @index = (@index + increment) % @images.size
    @image = @images[@index].image
    draw_main_window
  end
end

Shoes.app :width => Eigenface::WIDTH,
  :height => Eigenface::HEIGHT,
  :title => "Face Recognizer"
