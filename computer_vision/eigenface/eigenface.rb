require 'fileutils'

class ImageEntry < Struct.new( :image, :chosen_face, :faces ); end

class Eigenface < Shoes
  WIDTH = 800
  HEIGHT = 600

  IMAGE_DIR = "images"
  TRAIN_FILE = "train.txt"
  TRAIN_DATA = "facedata.xml"
  TEST_FILE = "test.txt"

  url '/', :index
  url '/next_image', :next_image

  def index
    @index = -1
    @images = Array.new
    @train_images = Array.new
    @test_images = Array.new
    draw_main_window

    keypress do |k|
      if k == :left
        next_image( -1 )
      end
      if k == :right
        next_image( 1 )
      end
    end
  end

  def draw_main_window
    self.clear
    stack do
      flow do
        button( "load training images" ) { open_folder( true ) }
        button( "load test images" ) { open_folder( false ) }
      end
      flow do
        button( "previous image" ) { next_image( -1 ) }
        button( "next image" ) { next_image( 1 ) }
        para "#{@index + 1} / #{@images.size} images"
      end
      para( "filename: #{@image}" ) if @image
      para "0 = Red, 1 = Orange, 2 = Yellow, 3 = Green"
      flow :margin_bottom => 5 do
        para "Face: "
        @face_number_entry = edit_box :height => 28, :width => 40
        if @image
          @face_number_entry.text = @images[@index].chosen_face
        end
        button( "set value" ) do
          set_value
          next_image( 1 )
        end
      end
      image @image if @image
    end
  end

  def detect_faces
    debug( @images.inspect )
    File.open( TRAIN_FILE, 'w' ) do |file|
      label = 1
      @train_images.each do |train_set|
        train_set.each do |image_entry|
          file.puts "#{label} #{image_entry.faces[image_entry.chosen_face]}"
        end

        label += 1
      end

      label = 1
      File.open( TEST_FILE, 'w' ) do |file|
        @test_images.each do |image_entry|
          file.puts "#{label} #{image_entry.image}"
        end
      end
    end

    # check that this file exists
    if not File.exist?( TRAIN_DATA )
      FileUtils.touch( TRAIN_DATA )
    end

    puts "./eigenface train"
    system "./eigenface train"
    puts "./eigenface test"
    system "./eigenface test"
  end

  def set_value
    @images[@index].chosen_face = @face_number_entry.text.to_i
  end

  def extract_faces( image )
    if not File.exist?( IMAGE_DIR )
      FileUtils.mkdir( IMAGE_DIR )
    end
    filename = image.split( "/" ).last
    filename_parts = filename.split('.')
    extension = filename_parts[1]
    output_filename = "#{IMAGE_DIR}/#{filename_parts[0]}_face.#{extension}"

    system( "./face_detect #{image} #{output_filename}" )
    files = Dir["#{output_filename.split('.')[0]}?.#{extension}"].sort

    [output_filename] + files
  end

  def open_folder( train )
    last_index = @images.size
    # pull images
    folder = ask_open_folder
    current_images = Dir["#{folder}/*.jpg"] + Dir["#{folder}/*.png"]
    current_images.sort!
    # extract faces
    if( train )
      current_images.collect! do |image|
        filenames = extract_faces( image )
        filename = filenames.shift

        ImageEntry.new( filename, nil, filenames )
      end
    else
      current_images.collect! do |image|
        ImageEntry.new( image, nil, nil )
      end
    end
    if( train )
      @train_images.push( current_images )
    else
      @test_images += current_images
      detect_faces

      current_images.collect! do |image|

        filename = image.image.split( "/" ).last
        filename_parts = filename.split('.')
        extension = filename_parts[1]
        output_filename = "#{IMAGE_DIR}/#{filename_parts[0]}_label.#{extension}"

        ImageEntry.new( output_filename, nil, nil )
      end
    end

    @images += current_images
    if @index == -1
      @index = 0
    else
      @index = last_index
    end
    debug( @images.inspect )
    @image = @images[@index].image

    self.clear
    draw_main_window
  end

  def next_image( increment )
    @index = (@index + increment) % @images.size
    @image = @images[@index].image
    draw_main_window
  end
end

Shoes.app :width => Eigenface::WIDTH,
  :height => Eigenface::HEIGHT,
  :title => "Face Recognizer"
